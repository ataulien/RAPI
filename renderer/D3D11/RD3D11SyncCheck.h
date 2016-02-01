#pragma once
#include "pch.h"

#ifdef RND_D3D11

#include "RTimer.h"
#include <map>
#include "Logger.h"
#include <assert.h>
#include "RTools.h"

#ifndef D3D11_GPU_SYNC_CHECK
//#define D3D11_GPU_SYNC_CHECK
#endif

namespace RAPI
{
	namespace __ctx_sync_check
	{
		struct __sync_check_stash
		{
			__sync_check_stash()
			{
				thread = GetCurrentThreadId();
			}

			struct Query
			{
				Query(ID3D11Device* device)
				{
					HRESULT hr;
					D3D11_QUERY_DESC desc;
					desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
					desc.MiscFlags = 0;

					LE(device->CreateQuery(&desc, &Disjoint));

					desc.Query = D3D11_QUERY_TIMESTAMP;

					LE(device->CreateQuery(&desc, &pTimestampBefore));
					LE(device->CreateQuery(&desc, &pTimestampAfter));

					CPUElapsed = 0.0;

					//SetHardwareBreakpoint(GetCurrentThread(), 
					//	HWBRK_TYPE_WRITE,HWBRK_SIZE_1,(*(void **)Disjoint));
				}

				~Query()
				{
					Clear();
				}

				void Clear()
				{
					SafeRelease(Disjoint);
					SafeRelease(pTimestampBefore);
					SafeRelease(pTimestampAfter);
				}

				void start(ID3D11DeviceContext* ctx)
				{
					ctx->Begin(Disjoint);
					ctx->End(pTimestampBefore);

					timer.Update();
				}

				void end(ID3D11DeviceContext* ctx)
				{
					timer.Update();
					CPUElapsed = timer.GetDeltaAccurate() * 1000.0;

					ctx->End(pTimestampAfter);
					ctx->End(Disjoint);
				}

				bool isDone(ID3D11DeviceContext* ctx, bool force)
				{
					UINT64 startTime, endTime;
					D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;

					if(!pTimestampBefore)
						return false;

					if(force)
					{
						while(ctx->GetData(pTimestampBefore, &startTime, sizeof(startTime), 0) != S_OK
							|| ctx->GetData(pTimestampAfter, &endTime, sizeof(endTime), 0) != S_OK
							|| ctx->GetData(Disjoint, &disjointData, sizeof(disjointData), 0) != S_OK);
					}

					if(ctx->GetData(pTimestampBefore, &startTime, sizeof(startTime), 0) == S_OK
						&& ctx->GetData(pTimestampAfter, &endTime, sizeof(endTime), 0) == S_OK
						&& ctx->GetData(Disjoint, &disjointData, sizeof(disjointData), 0) == S_OK
						&& disjointData.Disjoint == FALSE)
					{
						UINT64 delta = endTime - startTime;
						double frequency = static_cast<double>(disjointData.Frequency);
						GPUElapsed = (delta / frequency) * 1000.0;
						return true;
					}

					return false;
				}

				ID3D11Query* Disjoint;
				ID3D11Query* pTimestampBefore;
				ID3D11Query* pTimestampAfter;
				double CPUElapsed;
				double GPUElapsed;


				RTimer timer;
			};

			struct QueryResult
			{
				double CPUElapsed;
				double GPUElapsed;
			};

			Query& NewQuery(void* addr, ID3D11Device* device)
			{
				assert(thread == GetCurrentThreadId());

				PendingResults[addr].push_back(new Query(device));
				return *PendingResults[addr].back();
			}

			void CheckResults(ID3D11DeviceContext* ctx, void* addr, std::vector<QueryResult>& finished)
			{
				assert(thread == GetCurrentThreadId());

				std::vector<Query*> del;
				auto& p = PendingResults[addr];

				for(auto it = p.begin(); it != p.end(); it++)
				{
					if((*it) && (*it)->isDone(ctx, false))
					{
						QueryResult result;
						result.CPUElapsed = (*it)->CPUElapsed;
						result.GPUElapsed = (*it)->GPUElapsed;
						finished.push_back(result);

						(*it)->Clear();

						del.push_back((*it));

						it = p.erase(it);

						if(p.empty())
							break;
					}
				}

				RTools::DeleteElements(del);
			}

			void CheckSyncPoint(ID3D11DeviceContext* ctx, void* addr)
			{
				static double s_largestCPU = 0.0;
				static double s_largestGPU = 0.0;

				assert(thread == GetCurrentThreadId());

				std::vector<QueryResult> results;
				CheckResults(ctx, addr, results);

				//std::stringstream out;
				//out << "GPU-Sync summary for '0x" << std::hex << addr << "':\n";
				//OutputDebugStringA(out.str().c_str());

				for(auto& r : results)
				{
					/*std::stringstream out;

					out << std::fixed << " - CPU: " << r.CPUElapsed << ", GPU: " << r.GPUElapsed << "\n";
					OutputDebugStringA(out.str().c_str());
					out.clear();*/

					s_largestCPU = std::max(s_largestCPU, r.CPUElapsed);
					s_largestGPU = std::max(s_largestGPU, r.GPUElapsed);

					CPUTimeLost += r.CPUElapsed;
					GPUTimeLost += r.GPUElapsed;

					if(r.CPUElapsed > 0.03)
					{
						//LogWarn() << "Potential GPU-Sync-Point hit!";

					//	__debugbreak();
					}
				}


			}

			void GetTimesLostAndReset(float& tcpu, float& tgpu)
			{
				tcpu = (float)CPUTimeLost;
				tgpu = (float)GPUTimeLost;

				CPUTimeLost = 0;
				GPUTimeLost = 0;
			}

			double CPUTimeLost;
			double GPUTimeLost;
			std::map<void*, std::list<Query*>> PendingResults;
			int thread;
		};

		SELECTANY __sync_check_stash GlobalSyncCheckStash;

	}
}

#ifdef D3D11_GPU_SYNC_CHECK
#define RD3D11_CTX_SYNC_CHECK(x, device, ctx) { __ctx_sync_check::__sync_check_stash::Query& q = __ctx_sync_check::GlobalSyncCheckStash.NewQuery(this, device); \
	q.start(ctx); \
	{x;} \
	q.end(ctx); \
	__ctx_sync_check::GlobalSyncCheckStash.CheckSyncPoint(ctx, this); }





#define RD3D11_CTX_SYNC_CHECK_R(x){RD3D11_CTX_SYNC_CHECK(x, REngine::RenderingDevice->GetDevice(), REngine::RenderingDevice->GetThreadContext(GetCurrentThreadId()));}
#else
#define RD3D11_CTX_SYNC_CHECK(x, device, ctx) {x;}
#define RD3D11_CTX_SYNC_CHECK_R(x){x;}
#endif
#endif