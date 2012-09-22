#include "Bloco.h"

//************************************
// Method:    VCreateHandle
// FullName:  Resource::VCreateHandle
// Access:    virtual public 
// Returns:   ResHandle *
// Qualifier:
// Parameter: const char * buffer
// Parameter: unsigned int size
// Parameter: ResCache * pResCache
//************************************
ResHandle *Resource::VCreateHandle(const char *buffer, unsigned int size, ResCache *pResCache)
{
	return DEBUG_CLIENTBLOCK ResHandle(*this, (char*)buffer, size, pResCache);
}

ResourceZipFile::~ResourceZipFile()
{
	SAFE_DELETE(m_pZipFile);
}

//************************************
// Method:    VOpen
// FullName:  ResourceZipFile::VOpen
// Access:    virtual public 
// Returns:   bool
// Qualifier:
//************************************
bool ResourceZipFile::VOpen()
{
	m_pZipFile = DEBUG_CLIENTBLOCK ZipFile();
	if(m_pZipFile)
	{
		return m_pZipFile->Init(m_resFileName.c_str());
	}
	return false;
}

//************************************
// Method:    VGetResourceSize
// FullName:  ResourceZipFile::VGetResourceSize
// Access:    virtual public 
// Returns:   int
// Qualifier:
// Parameter: const Resource & r
//************************************
int ResourceZipFile::VGetResourceSize(const Resource &r)
{
	int size = 0;
	optional<int>	resourceNum = m_pZipFile->Find(r.m_name.c_str());
	if (resourceNum.valid())
	{
		size = m_pZipFile->GetFileLen(*resourceNum);
	}
	return size;
}

//************************************
// Method:    VGetResource
// FullName:  ResourceZipFile::VGetResource
// Access:    virtual public 
// Returns:   int
// Qualifier:
// Parameter: const Resource & r
// Parameter: char * buffer
//************************************
int ResourceZipFile::VGetResource(const Resource &r, char *buffer)
{
	int size = 0;
	optional<int>	resourceNum = m_pZipFile->Find(r.m_name.c_str());
	if (resourceNum.valid())
	{
		size = m_pZipFile->GetFileLen(*resourceNum);
		m_pZipFile->ReadFile(*resourceNum, buffer);
	}
	return size;
}


ResHandle::ResHandle(Resource & resource, char *buffer, unsigned int size, ResCache *pResCache)
	: m_resource(resource)
{
	m_buffer = buffer;
	m_size = size;
	m_pResCache = pResCache;
}
ResHandle::~ResHandle()
{
	if (m_buffer) delete [] m_buffer;
	m_pResCache->MemoryHasBeenFreed(m_size);
}


ResCache::ResCache(const unsigned int sizeInMb, IResourceFile *file)
{
	m_cacheSize = sizeInMb * MB;
	m_allocated = 0;
	m_file = file;
}
ResCache::~ResCache()
{
	while (!m_lru.empty())
	{
		FreeOneResource();
	}
	SAFE_DELETE(m_file);
}

//************************************
// Method:    GetHandle
// FullName:  ResCache::GetHandle
// Access:    public 
// Returns:   shared_ptr<ResHandle>
// Qualifier:
// Parameter: Resource * r
//************************************
shared_ptr<ResHandle> ResCache::GetHandle(Resource* r)
{
	shared_ptr<ResHandle> handle(Find(r));
	if (handle == NULL)
	{
		handle = Load(r);
	}
	else
	{
		Update(handle);
	}
	return handle;
}

//************************************
// Method:    Load
// FullName:  ResCache::Load
// Access:    protected 
// Returns:   shared_ptr<ResHandle>
// Qualifier:
// Parameter: Resource * r
//************************************
shared_ptr<ResHandle> ResCache::Load(Resource * r)
{
	int size = m_file->VGetResourceSize(*r);
	char* buffer = Allocate(size);
	if (buffer == NULL)
	{
		return shared_ptr<ResHandle>();  // Out of memory
	}

	// Create a DEBUG_CLIENTBLOCK resource and add it to the lru and map
	shared_ptr<ResHandle> handle (r->VCreateHandle(buffer, size, this));
	handle->VLoad(m_file);

	m_lru.push_front(handle);
	m_resources[r->m_name] = handle;

	return handle;
}

//************************************
// Method:    Find
// FullName:  ResCache::Find
// Access:    protected 
// Returns:   shared_ptr<ResHandle>
// Qualifier:
// Parameter: Resource * r
//************************************
shared_ptr<ResHandle> ResCache::Find(Resource * r)
{
	ResHandleMap::iterator i = m_resources.find(r->m_name);
	if(i==m_resources.end())
		return shared_ptr<ResHandle>();  // Out

	return (*i).second; 
}

//************************************
// Method:    Update
// FullName:  ResCache::Update
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: shared_ptr<ResHandle> handle
//************************************
void ResCache::Update(shared_ptr<ResHandle> handle)
{
	m_lru.remove(handle);
	m_lru.push_front(handle);
}

//************************************
// Method:    Allocate
// FullName:  ResCache::Allocate
// Access:    protected 
// Returns:   char*
// Qualifier:
// Parameter: unsigned int  size
//************************************
char* ResCache::Allocate(unsigned int size)
{
	if(!MakeRoom(size))
	{
		return NULL;
	}

	char* mem = DEBUG_CLIENTBLOCK char[size];
	if(mem)
		m_allocated += size;

	return mem;
}

//************************************
// Method:    FreeOneResource
// FullName:  ResCache::FreeOneResource
// Access:    protected 
// Returns:   void
// Qualifier:
//************************************
void ResCache::FreeOneResource()
{
	ResHandleList::iterator gonner = m_lru.end();
	gonner--;

	shared_ptr<ResHandle> handle = *gonner;

	m_lru.pop_back();
	m_resources.erase(handle->m_resource.m_name);
}

//************************************
// Method:    Flush
// FullName:  ResCache::Flush
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void ResCache::Flush()
{
	while(!m_lru.empty())
	{
		shared_ptr<ResHandle> handle = *(m_lru.begin());
		Free(handle);
		m_lru.pop_front();
	}
}

//************************************
// Method:    MakeRoom
// FullName:  ResCache::MakeRoom
// Access:    protected 
// Returns:   bool
// Qualifier:
// Parameter: unsigned int size
//************************************
bool ResCache::MakeRoom(unsigned int size)
{	
	if(size > m_cacheSize)
	{
		return false;
	}

	while(size >(m_cacheSize - m_allocated))
	{
		if (m_lru.empty())
			return false;

		FreeOneResource();
	}

	return true;
}

//************************************
// Method:    Free
// FullName:  ResCache::Free
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: shared_ptr<ResHandle> gonner
//************************************
void ResCache::Free(shared_ptr<ResHandle> gonner)
{
	m_lru.remove(gonner);
	m_resources.erase(gonner->m_resource.m_name);
	// Note - the resource might still be in use by something,
	// so the cache can't actually count the memory freed until the
	// ResHandle pointing to it is destroyed.

}

//************************************
// Method:    MemoryHasBeenFreed
// FullName:  ResCache::MemoryHasBeenFreed
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: unsigned int size
//************************************
void ResCache::MemoryHasBeenFreed(unsigned int size)
{
	m_allocated -= size;
}


