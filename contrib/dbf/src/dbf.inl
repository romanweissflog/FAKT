// dbf.inl
// Copyright (c) 2007-2015 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

inline CDBase::CDBase() : m_handle(NULL)
{
}

inline CDBase::~CDBase()
{
    if (IsOpen())
        Close();
}

inline bool CDBase::IsOpen() const
{ 
   return (m_handle != NULL);
}

inline bool CDBase::Attach(void* stream, const struct zlib_filefunc_def_s* api, dbf_editmode editmode, dbf_charconv conv, void* memo, const char* tablename)
{
    m_handle = ::dbf_attach(stream, api, editmode, conv, memo, tablename);
    return (m_handle != NULL);
}

inline bool CDBase::Attach(DBF_HANDLE handle)
{
    m_handle = handle;
    return (m_handle != NULL);
}

inline bool CDBase::Attach(CDBase* db)
{
   return Attach(db->Detach());
}

inline DBF_HANDLE CDBase::Detach()
{
   DBF_HANDLE handle = m_handle;
   m_handle = NULL;
   return handle;
}

inline bool CDBase::Open(const char* filename, dbf_editmode editmode, const char* tablename)
{
    DBF_OPEN parm;

    parm.api       = NULL;
    parm.memo      = false;
    parm.charconv  = dbf_charconv_default;
    parm.editmode  = editmode;
    parm.tablename = tablename;
    m_handle = ::dbf_open(filename, &parm);
    return (m_handle != NULL);
}

inline bool CDBase::Open(const char* filename, const DBF_OPEN& parm)
{
    m_handle = ::dbf_open(filename, &parm);
    return (m_handle != NULL);
}

inline bool CDBase::Create(void* stream, const DBaseCreateParm& parm, void* memoStream)
{
    DBF_HANDLE handle;
    DBF_CREATE temp;
    bool ok;

    temp.api        = parm.api;
    temp.array      = parm.vector.empty() ? NULL : &parm.vector[0];
    temp.array_count = parm.vector.size();
    temp.charconv   = parm.charconv;
    temp.tablename  = parm.tablename;
    temp.version    = parm.version;
    handle = ::dbf_create_attach(stream, &temp, memoStream);
    ok = (handle != NULL);
    if (ok)
        m_handle = handle;
    return ok;
}

inline bool CDBase::Create(const char* filePath, const DBaseCreateParm& parm)
{
    DBF_HANDLE handle;
    DBF_CREATE temp;
    bool ok;

    temp.api        = parm.api;
    temp.array      = parm.vector.empty() ? NULL : &parm.vector[0];
    temp.array_count = parm.vector.size();
    temp.charconv   = parm.charconv;
    temp.tablename  = parm.tablename;
    temp.version    = parm.version;

    handle = ::dbf_create(filePath, &temp);
    ok = (handle != NULL);
    if (ok)
        m_handle = handle;
    return ok;
}

inline void CDBase::Close()
{
    ::dbf_close(&m_handle);
}

inline void CDBase::CloseMemoFile()
{
    ::dbf_close_memo(m_handle);
}

inline bool CDBase::SetFlag(dbf_flag flag, bool set)
{ 
    return ::dbf_setflag(m_handle, flag, set) ? true : false;
}

inline dbf_uint CDBase::GetPosition() const
{ 
    return ::dbf_getposition(m_handle); 
}

inline dbf_uint CDBase::GetRecordCount() const
{ 
    return ::dbf_getrecordcount(m_handle); 
}

inline dbf_uint CDBase::GetFieldCount() const 
{ 
    return ::dbf_getfieldcount(m_handle);
}

inline bool CDBase::SetPosition(dbf_uint index)
{
    return ::dbf_setposition(m_handle, index) ? true : false;
}

inline bool CDBase::PutRecord(dbf_uint index)
{
    return ::dbf_putrecord(m_handle, index) ? true : false;
}

inline bool CDBase::PutRecord()
{
    return PutRecord(GetPosition());
}

inline size_t CDBase::Read(const DBF_FIELD* field, std::string* buf, size_t buf_len)
{
    char* temp = new char[buf_len];
    size_t n = ::dbf_getfield(m_handle, field, temp, buf_len, DBF_DATA_TYPE_ANY);
    *buf = temp;
    delete [] temp;
    return n;
}

inline size_t CDBase::Read(const char* field, std::string* buf, size_t buf_len)
{
    char* temp = new char[buf_len];
    size_t n = ::dbf_getfield(m_handle, GetFieldPtr(field), temp, buf_len, DBF_DATA_TYPE_ANY);
    *buf = temp;
    delete [] temp;
    return n;
}

inline bool CDBase::GetFirstRecord()
{
    bool ok = ::dbf_move_prepare(m_handle) ? true : false;
    if (ok)
        ok = SetPosition(0);
    return ok;
}

inline bool CDBase::GetLastRecord()
{
    bool ok = ::dbf_move_prepare(m_handle) ? true : false;
    if (ok)
        ok = SetPosition(GetRecordCount()-1);
    return ok;
}

inline bool CDBase::GetNextRecord()
{
    bool ok = ::dbf_move_prepare(m_handle) ? true : false;
    if (ok)
        ok = SetPosition(GetPosition() + 1);
    return ok;
}

inline bool CDBase::GetPrevRecord()
{
    bool ok = ::dbf_move_prepare(m_handle) ? true : false;
    if (ok)
        ok = SetPosition(GetPosition() - 1);
    return ok;
}

inline bool CDBase::Read(const DBF_FIELD* field, struct tm* tm, int* ms)
{
    return ::dbf_getfield_tm(m_handle, field, tm, ms) ? true : false;
}

inline bool CDBase::Read(const char* field, struct tm* tm, int* ms)
{
    return Read(GetFieldPtr(field), tm, ms);
}

inline bool CDBase::Read(dbf_uint field, struct tm* tm, int* ms)
{
    return Read(GetFieldPtr(field), tm, ms);
}

inline bool CDBase::ReadDateTime(const DBF_FIELD* field, time_t* utc, int* ms)
{
    return ::dbf_getfield_time(m_handle, field, utc, ms) ? true : false;
}

inline bool CDBase::ReadDateTime(const char* field, time_t* utc, int* ms)
{
    return ReadDateTime(GetFieldPtr(field), utc, ms);
}

inline bool CDBase::ReadDateTime(dbf_uint field, time_t* utc, int* ms)
{
    return ReadDateTime(GetFieldPtr(field), utc, ms);
}

#ifdef _WINBASE_
inline bool CDBase::Write(const DBF_FIELD* field, const SYSTEMTIME& st)
{
   struct tm tm;
   tm.tm_year = st.wYear - 1900;
   tm.tm_mon  = st.wMonth - 1;
   tm.tm_mday = st.wDay;
   tm.tm_hour = st.wHour;
   tm.tm_min  = st.wMinute;
   tm.tm_sec  = st.wSecond;
   tm.tm_isdst = 0;
   tm.tm_wday = 0;
   tm.tm_yday = 0;
   return Write(field, tm, st.wMilliseconds);
}
#endif

inline bool CDBase::Write(const DBF_FIELD* field, const struct tm& tm, int ms, dbf_data_type type)
{
    return ::dbf_putfield_tm(m_handle, field, &tm, ms, type) ? true : false;
}

inline bool CDBase::Write(const char* field, const struct tm& tm, int ms, dbf_data_type type)
{
    return Write(GetFieldPtr(field), tm, ms, type);
}

inline bool CDBase::Write(dbf_uint field, const struct tm& tm, int ms, dbf_data_type type)
{
    return Write(GetFieldPtr(field), tm, ms, type);
}

inline bool CDBase::WriteTime(const DBF_FIELD* field, time_t utc, int ms, dbf_data_type type)
{
    return ::dbf_putfield_time(m_handle, field, utc, ms, type) ? true : false;
}

inline bool CDBase::WriteTime(const char* field, time_t utc, int ms, dbf_data_type type)
{
    return WriteTime(GetFieldPtr(field), utc, ms, type);
}

inline bool CDBase::WriteTime(dbf_uint field, time_t utc, int ms, dbf_data_type type)
{
    return WriteTime(GetFieldPtr(field), utc, ms, type);
}

inline bool CDBase::Read(const DBF_FIELD* field, bool* b)
{
    BOOL temp;
    bool ok = ::dbf_getfield_bool(m_handle, field, &temp) ? true : false;
    if (ok)
        *b = temp ? true : false;
    return ok;
}

inline bool CDBase::Read(dbf_uint field, bool* b)
{
    return Read(GetFieldPtr(field), b);
}

inline bool CDBase::Read(const char* field, bool* b)
{
    return Read(GetFieldPtr(field), b);
}

inline bool CDBase::Write(const DBF_FIELD* field, const bool& b)
{
    return ::dbf_putfield_bool(m_handle, field, b) ? true : false;
}

inline bool CDBase::Write(const char* field, const bool& b)
{
    return Write(GetFieldPtr(field), b);
}

inline bool CDBase::Write(dbf_uint field, const bool& b)
{
   return Write(GetFieldPtr(field), b);
}

inline bool CDBase::Write(const DBF_FIELD* field, const std::string& str)
{
    return ::dbf_putfield(m_handle, field, str.c_str()) ? true : false;
}

inline bool CDBase::Write(const char* field, const std::string& str)
{
    return Write(GetFieldPtr(field), str);
}

inline bool CDBase::Write(dbf_uint field, const std::string& str)
{
    return Write(GetFieldPtr(field), str);
}

inline bool CDBase::AddNew()
{
    return ::dbf_addrecord(m_handle) ? true : false;
}

inline const DBF_FIELD* CDBase::GetFieldPtr(dbf_uint field) const
{
    return ::dbf_getfieldptr(m_handle, field);
}

inline const DBF_FIELD* CDBase::GetFieldPtr(const char* field) const
{
    return ::dbf_getfieldptr_name(m_handle, field);
}

inline size_t CDBase::Read(const DBF_FIELD* field, std::string* buf, size_t buf_len, dbf_data_type type) const
{
    char* temp = new char[buf_len];
    size_t n = ::dbf_getfield(m_handle, field, temp, buf_len, type);
    *buf = temp;
    delete [] temp;
    return n;
}

inline size_t CDBase::Read(dbf_uint field, std::string* buf, size_t buf_len, dbf_data_type type) const
{
    return Read(GetFieldPtr(field), buf, buf_len, type);
}

inline size_t CDBase::Read(const char* field, std::string* buf, size_t buf_len, dbf_data_type type) const
{
    return Read(GetFieldPtr(field), buf, buf_len, type);
}

inline bool CDBase::IsRecordDeleted() const
{
   return ::dbf_isrecorddeleted(m_handle) ? true : false;
} 

inline bool CDBase::DeleteRecord(bool bDelete)
{
    return ::dbf_deleterecord(m_handle, bDelete) ? true : false;
}

inline bool CDBase::IsEmpty() const
{
    return (0 == GetRecordCount());
}

inline bool CDBase::Copy(void* stream, void* stream_memo, const struct zlib_filefunc_def_s* api, bool include_records, bool include_deleted_records)
{
    return ::dbf_copy(m_handle, stream, stream_memo, api, include_records, include_deleted_records) ? true : false;
}

inline int CDBase::GetLastError() const
{
    return ::dbf_getlasterror(m_handle);
}

inline const char* CDBase::GetLastErrorStr() const
{
    return ::dbf_getlasterror_str(m_handle);
}

inline void CDBase::GetInfo(DBF_INFO* info) const
{
    ::dbf_getinfo(m_handle, info);
}

inline bool CDBase::GetFieldInfo(dbf_uint index, DBF_FIELD_INFO* info) const
{
    return ::dbf_getfield_info(m_handle, index, info) ? true : false;
}

inline dbf_data_type CDBase::GetFieldType(const DBF_FIELD* field) const
{
    return ::dbf_getfield_type(m_handle, field);
}

inline dbf_data_type CDBase::GetFieldType(dbf_uint field) const
{
    return ::dbf_getfield_type(m_handle, GetFieldPtr(field));
}

inline bool CDBase::OpenMemo(void* stream)
{
    return ::dbf_memo_open(m_handle, stream) ? true : false;
}
/*
inline bool CDBase::CreateMemo(void* stream)
{
    return ::dbf_memo_create(m_handle, stream) ? true : false;
}
*/

inline void CDBase::GetFileAPI(struct zlib_filefunc_def_s* api) const
{
    ::dbf_getfileapi(m_handle, api);
}

inline void* CDBase::GetMemoFile()
{
    return ::dbf_getmemofile(m_handle);
}

inline bool CDBase::IsEditable() const
{
    return (dbf_editmode_editable == ::dbf_geteditmode(m_handle));
}

inline bool CDBase::IsModified() const
{
    return ::dbf_ismodified(m_handle) ? true : false;
}

inline bool CDBase::IsFieldNull(const DBF_FIELD* field) const
{
    return ::dbf_isnull(m_handle, field) ? true : false;
}

inline bool CDBase::Read(const DBF_FIELD* field, long* n)
{
    return ::dbf_getfield_numeric(m_handle, field, n) ? true : false;
}

inline bool CDBase::Read(const char* field, long* n)
{
    return ::dbf_getfield_numeric(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Read(dbf_uint field, long* n)
{
    return ::dbf_getfield_numeric(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Read(const DBF_FIELD* field, double* n)
{
    return ::dbf_getfield_float(m_handle, field, n) ? true : false;
}

inline bool CDBase::Read(const char* field, double* n)
{
    return ::dbf_getfield_float(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Read(dbf_uint field, double* n)
{
    return ::dbf_getfield_float(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Write(const char* field, const long& n)
{
    return ::dbf_putfield_numeric(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Write(const DBF_FIELD* field, const long& n)
{
    return ::dbf_putfield_numeric(m_handle, field, n) ? true : false;
}

inline bool CDBase::Write(dbf_uint field, const long& n)
{
    return ::dbf_putfield_numeric(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Write(const char* field, const double& n)
{
    return ::dbf_putfield_float(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Write(const DBF_FIELD* field, const double& n)
{
    return ::dbf_putfield_float(m_handle, field, n) ? true : false;
}

inline bool CDBase::Write(dbf_uint field, const double& n)
{
    return ::dbf_putfield_float(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline int CDBase::FindField(const char* fieldname) const
{
    return ::dbf_findfield(m_handle, fieldname);
}

inline bool CDBase::ParseDate(const char* str, struct tm* tm, int* ms, dbf_data_type type)
{
    return ::dbf_parsedate(str, tm, ms, type) ? true : false;
}
