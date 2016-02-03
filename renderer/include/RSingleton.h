#pragma once

namespace RAPI
{
    namespace RTools
    {
        template<typename Signature>
        class RSingleton
        {
#define SINGLETONCLASS(classdef) friend class RTools::RSingleton<classdef>;
        public:

            static void createSingleton()
            {
                if (!m_pSingleton)
                    m_pSingleton = new Signature();
            }

            static Signature *getSingletonPtr()
            {
                if (!m_pSingleton)
                    m_pSingleton = new Signature();

                return m_pSingleton;
            }

            static Signature &getSingleton()
            {
                return *getSingletonPtr();
            }

            static void resetInstance()
            {
                if (m_pSingleton)
                {
                    delete m_pSingleton;
                    m_pSingleton = nullptr;
                }
            }

        private:
            static Signature *m_pSingleton;

            RSingleton(const RSingleton<Signature> &)
            { }

        protected:
            RSingleton()
            { }
        };

        template<typename Signature> Signature *RSingleton<Signature>::m_pSingleton = nullptr;
    }
}