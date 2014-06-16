#ifndef _NVPMAPI_MANAGER_H_
#define _NVPMAPI_MANAGER_H_

#include <windows.h>
#include <assert.h>
#include <NvPmApi.h>

//  For lifetime manager of NVPMAPI DLL
template<class EXPORTED_API> class InterfaceManager
{
public:
    InterfaceManager(NVPM_UUID InterfaceUUID) : m_hModule(0), m_InterfaceUUID(InterfaceUUID)
    {
        ClearApiTables();
    }
        
    ~InterfaceManager()
    {
        if (m_hModule)
        {
            ClearApiTables();

            FreeLibrary(m_hModule);
            m_hModule = 0;
        }
    }

    HRESULT Construct(const wchar_t* pLibraryPath)
    {
        if(pLibraryPath == NULL)
        {
            return E_INVALIDARG;
        }

        if((m_hModule = LoadLibraryW(pLibraryPath)) == NULL)
        {
            return E_FAIL;
        }

        NVPMGetExportTable_Pfn pfnGetExportTable = (NVPMGetExportTable_Pfn)GetProcAddress(m_hModule, "NVPMGetExportTable");
        if(pfnGetExportTable == NULL)
        {
            return E_FAIL;
        }
            
        if(pfnGetExportTable(&m_InterfaceUUID, (void **) &m_pNvPmApi) != S_OK)
        {
            return E_FAIL;
        }

        return S_OK;
    }

    const EXPORTED_API* Api() const
    {
        return m_pNvPmApi;
    }

private:
    HMODULE m_hModule;
    const EXPORTED_API *m_pNvPmApi;
    NVPM_UUID m_InterfaceUUID;
        
private:
    void ClearApiTables()
    {
        m_pNvPmApi = NULL;
    }
};

class NvPmApiManager : public InterfaceManager<NvPmApi>
{
public:
    NvPmApiManager() : InterfaceManager(ETID_NvPmApi) {}
};

#endif