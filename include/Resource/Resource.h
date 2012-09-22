#ifndef Resource_h__
#define Resource_h__

#include "Bloco.h"

using namespace std;

class ResHandle;
class ResCache;

class Resource
{
public:
	string m_name;

	Resource(string	name) {m_name = name;}
	virtual ResHandle* VCreateHandle(const char *buffer, unsigned int size, ResCache* pResCache);

};

class ZipFile;

class ResourceZipFile : public IResourceFile
{
	ZipFile* m_pZipFile;
	wstring m_resFileName;

public:
	ResourceZipFile(const PWSTR resFileName) {m_pZipFile = NULL; m_resFileName = resFileName;}
	virtual ~ResourceZipFile();

	virtual bool VOpen();
	virtual int VGetResourceSize(const Resource &r);
	virtual int VGetResource(const Resource &r, char *buffer);
};

typedef shared_ptr<ResHandle> ResHandlePtr;
class ResHandle
{
	friend class ResCache;

protected:
	Resource m_resource;
	char*	 m_buffer;
	unsigned int m_size;

	ResCache* m_pResCache;

public:
	ResHandle(Resource& resource, char* buffer, unsigned int size, ResCache* pResCache);
	virtual ~ResHandle();
	virtual int VLoad(IResourceFile* file)	{return file->VGetResource(m_resource, m_buffer); }
	unsigned int Size() const {return m_size;}
	char* Buffer() const {return m_buffer;}
};

typedef list<shared_ptr<ResHandle>> ResHandleList;			// lru (least recently used)
typedef map<string , shared_ptr<ResHandle>> ResHandleMap;	// maps identifiers to resource data

class ResCache
{
	friend class ResHandle;

	ResHandleList m_lru;
	ResHandleMap m_resources;
	IResourceFile* m_file;

	unsigned int m_cacheSize;
	unsigned int m_allocated;

protected:
	bool MakeRoom(unsigned int size);
	char* Allocate(unsigned int size);
	void Free(shared_ptr<ResHandle> handle);

	shared_ptr<ResHandle> Load(Resource* r);
	shared_ptr<ResHandle> Find(Resource* r);
	

	void FreeOneResource();
	void MemoryHasBeenFreed(unsigned int size);

public:
	ResCache(const unsigned int sizeInMb, IResourceFile* file);
	virtual ~ResCache();

	bool Init()	{return m_file->VOpen(); }
	shared_ptr<ResHandle> GetHandle(Resource* r);
	void Update(shared_ptr<ResHandle> handle);

	void Flush();
};

#endif // Resource_h__
