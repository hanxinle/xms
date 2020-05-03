#include "xtools.h"

#include <openssl/md5.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include "xmsg_com.pb.h"

#ifdef _WIN32
#include <io.h>
#include <windows.h>  //��protobuf�г�ͻ��Ҫ�ŵ��������
#include <direct.h>
#else
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/vfs.h>
#define _access access
#define _mkdir(d) mkdir(d,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)

#endif

using namespace std;

using namespace xmsg;

bool XMutex::is_debug = false;
static int mutex_index = 0;
static int lock_count = 0;
static int unlock_count = 0;

XMutex::XMutex(std::mutex *mux)
{
    mux_ = mux;
    mux->lock();
    mutex_index++;
    lock_count++;
    this->index_ = mutex_index;
    if (is_debug)
        cout << index_ << "|" << msg_ << ":Lock" << "L/U(" << lock_count << "/" << unlock_count << ")" << endl;
}

XMutex::XMutex(std::mutex *mux, std::string msg)
{
    mux_ = mux;
    msg_ = msg;
    mux->lock();
    mutex_index++;
    lock_count++;
    this->index_ = mutex_index;
    if (is_debug)
        cout << index_<<"|"<< msg_ << ":Lock"<<"L/U("<< lock_count <<"/"<< unlock_count <<")" << endl;
}

XMutex::~XMutex()
{
    mux_->unlock();
    unlock_count++;
    if (is_debug)
        cout << index_ << "|" << msg_ << ":UnLock" << "L/U(" << lock_count << "/" << unlock_count << ")" << endl;
}

void PrintMsg(XMsgHead *head, XMsg *msg)
{
    //stringstream ss;
    //ss << "��MSG��";
    //if (head)
    //{
    //    ss << head->service_name();
    //}
    //cout << "��MSG��" << pb_head_->service_name() << " " << msg->size << " " << msg->type << endl;
    //if (msg)
    //{
    //    int32 msg_size = 1;

    //    //��Ϣ����
    //    MsgType msg_type = 2;

    //    //���� ���ʱ��½��Ϣ��δ��
    //    string token = 3;

    //    //΢��������ƣ�����·��
    //    string service_name = 4;
    //}
}
int Base64Encode(const unsigned char *in, int len, char *out_base64)
{
    if (!in || len <= 0 || !out_base64)
        return 0;
    // Դ �ڴ�BIO
    auto mem_bio = BIO_new(BIO_s_mem());
    if (!mem_bio)return 0;

    //���� base64 BIO 
    auto b64_bio = BIO_new(BIO_f_base64());
    if (!b64_bio)
    {
        BIO_free(mem_bio);
        return 0;
    }
    //BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);

    //�γ�BIO�� b64-mem
    BIO_push(b64_bio, mem_bio);

    //����ͷд�� base64����󴫵���β
    BIO_write(b64_bio, in, len);
    
    //ˢ�»��壬д�뵽�������һ��BIO
    BIO_flush(b64_bio);

    BUF_MEM *p_data;

    //��ȡ�ڴ�BIO������ 
    BIO_get_mem_ptr(b64_bio, &p_data);

    memcpy(out_base64, p_data->data, p_data->length );
    //out_base64[p_data->length - 1] = 0;
    BIO_free_all(b64_bio);
    return p_data->length;
}

int Base64Decode(const char *in, int len, unsigned char *out_data)
{
    if (!in || len <= 0 || !out_data)
        return 0;
    // Դ �ڴ�BIO
    auto mem_bio = BIO_new_mem_buf(in, len+1);
    if (!mem_bio)return 0;

    //���� base64 BIO
    auto b64_bio = BIO_new(BIO_f_base64());
    if (!b64_bio)return 0;
   // if(in[len-1] !='\0')
    //BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);
    //�γ�BIO�� b64-mem
    b64_bio = BIO_push(b64_bio, mem_bio);
    BIO_flush(b64_bio);
    size_t size = 0;
    BIO_read_ex(b64_bio, out_data, len, &size);
    
    BIO_free_all(mem_bio);

    return size;
}

//����md5 128bit(16�ֽ�) 
unsigned char *XMD5(const unsigned char *d, unsigned long n, unsigned char *md)
{
    return MD5(d, n, md);
   
}

//����md5 128bit(16�ֽ�) �پ���base64ת��Ϊ�ַ���
std::string XMD5_base64(const unsigned char *d, unsigned long n)
{
    unsigned char buf[16] = { 0 };
    char base64[25] = { 0 };
    XMD5(d, n, buf);
    Base64Encode(buf, 16, base64);
    base64[24] = '\0';
    return base64;
}

XCOM_API std::string XGetIconFilename(std::string filename, bool is_dir)
{
    string iconpath = "Other";
    ///�ļ�����
    string filetype = "";
    int pos = filename.find_last_of('.');
    if (pos > 0)
    {
        filetype = filename.substr(pos + 1);
    }
    //ת��ΪСд �����������������
    transform(filetype.begin(), filetype.end(), filetype.begin(), ::tolower);

    if (is_dir)
    {
        iconpath = "Folder";
    }
    else if (filetype == "jpg" || filetype == "png" || filetype == "gif")
    {
        iconpath = "Img";
    }
    else if (filetype == "doc" || filetype == "docx" || filetype == "wps")
    {
        iconpath = "Doc";
    }
    else if (filetype == "rar" || filetype == "zip" || filetype == "7z" || filetype == "gzip")
    {
        iconpath = "Rar";
    }
    else if (filetype == "ppt" || filetype == "pptx")
    {
        iconpath = "Ppt";
    }
    else if (filetype == "xls" || filetype == "xlsx")
    {
        iconpath = "Xls";
    }
    else if (filetype == "pdf")
    {
        iconpath = "Pdf";
    }
    else if (filetype == "doc" || filetype == "docx" || filetype == "wps")
    {
        iconpath = "Doc";
    }
    else if (filetype == "avi" || filetype == "mp4" || filetype == "mov" || filetype == "wmv")
    {
        iconpath = "Video";
    }
    else if (filetype == "mp3" || filetype == "pcm" || filetype == "wav" || filetype=="wma")
    {
        iconpath = "Music";
    }
    else
    {
        iconpath = "Other";
    }
    return iconpath;
}
XCOM_API std::string XGetSizeString(long long size)
{
    string filesize_str = "";
    if (size > 1024 * 1024 * 1024) //GB
    {
        double gb_size = (double)size / (double)(1024 * 1024 * 1024);
        long long tmp = gb_size * 100;

        stringstream ss;
        ss << tmp / 100;
        if (tmp % 100 > 0)
            ss << "." << tmp % 100;
        ss << "GB";
        filesize_str = ss.str();
    }
    else if (size > 1024 * 1024) //MB
    {
        double gb_size = (double)size / (double)(1024 * 1024);
        long long tmp = gb_size * 100;

        stringstream ss;
        ss << tmp / 100;
        if (tmp % 100 > 0)
            ss << "." << tmp % 100;
        ss << "MB";
        filesize_str = ss.str();
    }
    else if (size > 1024) //KB
    {
        float gb_size = (float)size / (float)(1024);
        long long tmp = gb_size * 100;
        stringstream ss;
        ss << tmp / 100;
        if (tmp % 100 > 0)
            ss << "." << tmp % 100;
        ss << "KB";
        filesize_str = ss.str();
    }
    else //B
    {
        float gb_size = size / (float)(1024);
        long long tmp = gb_size * 100;

        stringstream ss;
        ss << size;
        ss << "B";
        filesize_str = ss.str();
    }
    return filesize_str;
}
XCOM_API std::string XGetTime(int timestamp, std::string fmt )
{
    char time_buf[128] = { 0 };
    time_t tm = 0;
    if (timestamp > 0)
        tm = timestamp;
    else
        tm = time(0);
    
    strftime(time_buf, sizeof(time_buf), fmt.c_str(), gmtime(&tm));
    return time_buf;
}
XCOM_API  void XStringSplit(std::vector<string> &vec, std::string str, std::string find)
{
    int pos1 = 0;
    int pos2 = 0;
    vec.clear();
    while ((pos2 = str.find(find, pos1)) != (int)string::npos)
    {
        vec.push_back(str.substr(pos1, pos2 - pos1));
        pos1 = pos2 + find.length();
    }
    string strTemp = str.substr(pos1);
    if ((int)strTemp.size() > 0)
    {
        vec.push_back(str.substr(pos1));
    }
}

XCOM_API   void XStrReplace(string str, string str_find, string str_replace, string &sreturn)
{
    sreturn = "";
    int str_size = str.size();
    int rep_size = str_find.size();
    for (int i = 0; i < str_size - (rep_size - 1); i++)
    {
        bool is_find = true;
        for (int j = 0; j < rep_size; j++)
        {
            if (str[i + j] != str_find[j])
            {
                is_find = false;
                break;
            }
        }
        if (is_find)
        {
            sreturn += str_replace;
            i += (rep_size - 1);
        }
        else
        {
            sreturn += str[i];
        }
    }
}
XCOM_API  std::string XFormatDir(const std::string &dir)
{
    std::string re = "";
    bool is_sep = false; // �Ƿ���/ ������ "\"
    for (int i = 0; i < dir.size(); i++)
    {
        if (dir[i] == '/' || dir[i] == '\\')
        {
            if (is_sep)
            {
                continue;
            }
            re += '/';
            is_sep = true;
            continue;
        }
        is_sep = false;
        re += dir[i];
    }
    return re;
}

XCOM_API string XTrim(const string& s)
{
    if (s.length() == 0)
        return s;
    int beg = s.find_first_not_of(" \a\b\f\n\r\t\v");
    int end = s.find_last_not_of(" \a\b\f\n\r\t\v");
    if (beg == (int)std::string::npos) // No non-spaces
        return "";
    return string(s, beg, end - beg + 1);
}

XCOM_API bool XFileExist(const std::string& s)
{
    if (_access(s.c_str(), 0) == -1)
    {
        return false;
    }
    return true;
}

XCOM_API void XNewDir(std::string path)
{
    string tmp = XFormatDir(path);

    vector<string>paths;
    XStringSplit(paths, tmp, "/");

    string tmpstr = "";
    for (auto s : paths)
    {
        tmpstr += s + "/";
        if (_access(tmpstr.c_str(), 0) == -1)
        {
            _mkdir(tmpstr.c_str());
        }
    }
}

XCOM_API void XDelFile(std::string path)
{
#ifdef _WIN32
    DeleteFileA(path.c_str());
#else
    remove(path.c_str());
#endif
}
//��ȡĿ¼�б�   //��ʽ �ļ������ļ���С��byte�����Ƿ���Ŀ¼��0,1�����ļ��޸�ʱ�䣨2020-01-22 19:30:13����
XCOM_API std::list< XToolFileInfo > GetDirList(std::string path)
{
    std::list< XToolFileInfo > file_list;

#ifdef _WIN32
    //�洢�ļ���Ϣ
    _finddata_t file;
    string dirpath = path + "/*.*";
    //Ŀ¼������
    intptr_t dir = _findfirst(dirpath.c_str(), &file);
    if (dir < 0)
        return file_list;
    char time_buf[128] = { 0 };
    do
    {
        XToolFileInfo file_info;
        if (file.attrib & _A_SUBDIR)
        {
            file_info.is_dir = true;
        }
        file_info.filename = file.name;
        file_info.filesize = file.size;
        file_info.time_write = file.time_write;
        /*
        
        %a ���ڼ��ļ�д
%A ���ڼ���ȫ��
%b �·ֵļ�д
%B �·ݵ�ȫ��
%c ��׼�����ڵ�ʱ�䴮
%C ��ݵĺ���λ����
%d ʮ���Ʊ�ʾ��ÿ�µĵڼ���
%D ��/��/��
%e �����ַ����У�ʮ���Ʊ�ʾ��ÿ�µĵڼ���
%F ��-��-��
%g ��ݵĺ���λ���֣�ʹ�û����ܵ���
%G ��֣�ʹ�û����ܵ���
%h ��д���·���
%H 24Сʱ�Ƶ�Сʱ
%I 12Сʱ�Ƶ�Сʱ
%j ʮ���Ʊ�ʾ��ÿ��ĵڼ���
%m ʮ���Ʊ�ʾ���·�
%M ʮʱ�Ʊ�ʾ�ķ�����
%n ���з�
%p ���ص�AM��PM�ĵȼ���ʾ
%r 12Сʱ��ʱ��
%R ��ʾСʱ�ͷ��ӣ�hh:mm
%S ʮ���Ƶ�����
%t ˮƽ�Ʊ��
%T ��ʾʱ���룺hh:mm:ss
%u ÿ�ܵĵڼ��죬����һΪ��һ�� ��ֵ��0��6������һΪ0��
%U ����ĵڼ��ܣ�����������Ϊ��һ�죨ֵ��0��53��
%V ÿ��ĵڼ��ܣ�ʹ�û����ܵ���
%w ʮ���Ʊ�ʾ�����ڼ���ֵ��0��6��������Ϊ0��
%W ÿ��ĵڼ��ܣ�������һ��Ϊ��һ�죨ֵ��0��53��
%x ��׼�����ڴ�
%X ��׼��ʱ�䴮
%y �������͵�ʮ������ݣ�ֵ��0��99��
%Y �����Ͳ��ֵ�ʮ�����
%z��%Z ʱ�����ƣ�������ܵõ�ʱ�������򷵻ؿ��ַ���
%% �ٷֺ�����ĳ�������ʾ��ǰ���������ڣ�

        */
        time_t tm = file_info.time_write;
        strftime(time_buf, sizeof(time_buf), "%F %R", gmtime(&tm));
        file_info.time_str = time_buf;
        file_list.push_back(file_info);
    } while (_findnext(dir, &file) == 0);
    _findclose(dir);
#else
    const char *dir = path.c_str();
    DIR *dp = 0;
    struct dirent *entry = 0;
    struct stat statbuf;
    dp = opendir(dir);
    if (dp == NULL)
        return file_list;
    chdir(dir);
    char buf[1024] = { 0 };
    while ((entry = readdir(dp)) != NULL)
    {
        XToolFileInfo file_info;
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            file_info.is_dir = true;
        }
        file_info.filename = entry->d_name;
        file_info.filesize = statbuf.st_size;
        /*
               struct timespec st_atim;  // Time of last access 
               struct timespec st_mtim;  // Time of last modification 
               struct timespec st_ctim;  // Time of last status change 

           #define st_atime st_atim.tv_sec      // Backward compatibility 
           #define st_mtime st_mtim.tv_sec
           #define st_ctime st_ctim.tv_sec
        */
        file_info.time_write = statbuf.st_mtime;
        time_t tm = file_info.time_write;
        char time_buf[16] = {0};
        strftime(time_buf, sizeof(time_buf), "%F %R", gmtime(&tm));
        file_info.time_str = time_buf;
        file_list.push_back(file_info);

        //sprintf(buf, "%s,%ld;", entry->d_name, statbuf.st_size);
        //data += buf;
    }
    closedir(dp);
#endif

    return file_list;
}
XCOM_API std::string GetDirData(std::string path)
{
    string data = "";
#ifdef _WIN32
    //�洢�ļ���Ϣ
    _finddata_t file;
    string dirpath = path + "/*.*";
    //Ŀ¼������
    intptr_t dir = _findfirst(dirpath.c_str(), &file);
    if (dir < 0)
        return data;
    do
    {
        if (file.attrib & _A_SUBDIR) continue;
        char buf[1024] = { 0 };
        sprintf(buf, "%s,%u;", file.name, file.size);
        data += buf;
    } while (_findnext(dir, &file) == 0);
    _findclose(dir);
#else
    const char *dir = path.c_str();
    DIR *dp = 0;
    struct dirent *entry = 0;
    struct stat statbuf;
    dp = opendir(dir);
    if(dp == NULL)
	    return data;
    chdir(dir);
    char buf[1024] = {0};
    while((entry = readdir(dp))!=NULL)
    {
	    lstat(entry->d_name,&statbuf);
	    if(S_ISDIR(statbuf.st_mode))continue;
	    sprintf(buf,"%s,%ld;",entry->d_name,statbuf.st_size);
	    data += buf;
    }
    closedir(dp);
#endif
    //ȥ����β ;
    if (!data.empty())
    {
        data = data.substr(0, data.size() - 1);
    }
    return data;
}

long long GetDirSize(const char * path)
{
    if (!path)return 0;
    long long dir_size = 0;
    string dir_new = path;
    string name = "";
#ifdef _WIN32
    //�洢�ļ���Ϣ
    _finddata_t file;
    dir_new += "\\*.*";

    intptr_t dir = _findfirst(dir_new.c_str(), &file);
    if (dir < 0)
        return 0;
    do
    {
        // �Ƿ����ļ��У��������Ʋ�Ϊ"."��".." 
        if (file.attrib & _A_SUBDIR)
        {
            name = file.name;
            if (name == "." || name == "..")
                continue;
            // ��dirNew����Ϊ��������Ŀ¼����������һ������ 
            dir_new = path;
            dir_new += "/";
            dir_new += name;
            dir_size += GetDirSize(dir_new.c_str());

        }
        else
        {
            dir_size += file.size;
        }
    } while (_findnext(dir, &file) == 0);
    _findclose(dir);
#else
    DIR *dp = 0;
    struct dirent *entry = 0;
    struct stat statbuf;
    dp = opendir(dir_new.c_str());
    if (dp == NULL)
        return 0;
    chdir(dir_new.c_str());
    char buf[1024] = { 0 };
    while ((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            name = entry->d_name;
            if (name == "." || name == "..")
                continue;
            dir_new = path;
            dir_new += "/";
            dir_new += entry->d_name;
            dir_size += GetDirSize(dir_new.c_str());
        }
        else
        {
            dir_size += statbuf.st_size;
        }
    }
    closedir(dp);
#endif
    return dir_size;
}
bool GetDiskSize(const char *dir, unsigned long long *avail, unsigned long long *total, unsigned long long *free)
{
#ifdef _WIN32

    return GetDiskFreeSpaceExA(dir, (ULARGE_INTEGER *)avail, (ULARGE_INTEGER *)total, (ULARGE_INTEGER *)free);
#else
    struct statfs diskInfo;
    statfs(dir, &diskInfo);
    *total = diskInfo.f_blocks*diskInfo.f_bsize;
    *free = diskInfo.f_bfree*diskInfo.f_bsize;
    *avail = diskInfo.f_bavail*diskInfo.f_bsize;
    return true;
#endif
}
//ͨ������ ����IP��ַ ֻȡ��һ��
std::string XGetHostByName(std::string host_name)
{
    #ifdef _WIN32
    static bool is_init = false;
    if (!is_init)
    {
        
        WORD sockVersion = MAKEWORD(2, 2);
        WSADATA wsaData;
        if (WSAStartup(sockVersion, &wsaData) != 0)
        {
            return "";
        }
        is_init = true;
    }
    #endif
    auto host = gethostbyname(host_name.c_str());
    auto addr = host->h_addr_list;
    if (!addr)
        return "";
    return inet_ntoa(*(struct in_addr*)*addr);
}


/*

void AES_ecb_encrypt(const unsigned char *in, unsigned char *out,
                     const AES_KEY *key, const int enc)
{

    assert(in && out && key);
    assert((AES_ENCRYPT == enc) || (AES_DECRYPT == enc));

    if (AES_ENCRYPT == enc)
        AES_encrypt(in, out, key);
    else
        AES_decrypt(in, out, key);
}
*/
class CXAES :public XAES
{
    ///////////////////////////////////////////////////////
    /// ���ü�����Կ ��Կ���� 128λ��16�ֽڣ� 192λ ��24�ֽڣ� 256λ (32�ֽ�)
    /// ���Ȳ��ܳ���32�ֽڣ�����ʧ��
    /// ��Կ�����Զ�����
    /// @key ��Կ
    /// @key_size ��Կ���� �ֽ�
    /// @is_enc true  ���� false ����
    /// @return ���óɹ�ʧ��
    virtual bool SetKey(const char *key, int key_size, bool is_enc) override
    {
        if (key_size > 32 || key_size <= 0)
        {
            cerr << "AES key size error(>32 && <=0 )! key_size= " << key_size << endl;
            return false;
        }
        unsigned char aes_key[32] = { 0 };
        memcpy(aes_key, key, key_size);
        int bit_size = 0;
        if (key_size > 24)
        {
            bit_size = 32 * 8;
        }
        else if (key_size > 16)
        {
            bit_size = 24 * 8;
        }
        else
        {
            bit_size = 16 * 8;
        }

        /*
            if (bits != 128 && bits != 192 && bits != 256)
             return -2;
        */
        if (is_enc)
        {
            is_set_encode = true;
            if (AES_set_encrypt_key(aes_key, bit_size, &aes_) < 0)
            {
                return false;
            }
            return true;
        }
        is_set_decode = true;
        if (AES_set_decrypt_key(aes_key, bit_size, &aes_) < 0)
        {
            return false;
        }
        return true;
    }

    virtual void Drop() override
    {
        delete this;
    }

    ///////////////////////////////////////////////////////
    /// ����
    /// @in ��������
    /// @in_size �������ݴ�С
    /// @out ��� ���ݿռ�Ҫ��֤16�ֽڵı���
    /// @return  �����С��ʧ�ܷ���<=0
    virtual long long Encrypt(const unsigned char *in, long long in_size, unsigned char *out) override
    {
        if (!in || in_size <= 0 || !out)
        {
            cerr << "Encrypt input data error" << endl;
            return 0;
        }

        if (!is_set_encode)
        {
            cerr << "Encrypt password not set" << endl;
            return 0;
        }
        long long enc_byte = 0;
        //AES_encrypt Ĭ�� AES_ecb_encrypt �ɷֿ鲢�м���  cbc����ǿ�ȸ���ÿ��ʱ������һ�����
        /*
        ÿ�μ���һ�����ݿ�16���ֽڣ���ȫ����Ҫ����
            s0 = GETU32(in     ) ^ rk[0];
            s1 = GETU32(in +  4) ^ rk[1];
            s2 = GETU32(in +  8) ^ rk[2];
            s3 = GETU32(in + 12) ^ rk[3];
        */
        unsigned char *p_in = 0;
        unsigned char *p_out = 0;
        unsigned char data[16] = { 0 };
        for (int i = 0; i < in_size; i += 16)
        {
            p_in = (unsigned char *)in + i;
            p_out = out + i;
            //������16�ֽڵĲ�ȫ
            if (in_size - i < 16)
            {
                memcpy(data, p_in, in_size - i);
                p_in = data;
            }
            enc_byte += 16;
            AES_encrypt(p_in, p_out, &aes_);
        }
        return enc_byte;
    }

    ///////////////////////////////////////////////////////
    /// ����
    /// @in ��������
    /// @in_size �������ݴ�С
    /// @out ��� ���ݿռ�Ҫ��֤16�ֽڵı���
    /// @return  �����С��ʧ�ܷ���<=0
    virtual long long Decrypt(const unsigned char *in, long long in_size, unsigned char *out) override
    {

        if (!in || in_size <= 0 || !out || in_size % 16 != 0)
        {
            cerr << "Decrypt input data error" << endl;
            return 0;
        }

        if (!is_set_decode)
        {
            cerr << "Decrypt password not set" << endl;
            return 0;
        }

        long long enc_byte = 0;
        //AES_encrypt Ĭ�� AES_ecb_encrypt �ɷֿ鲢�м���  cbc����ǿ�ȸ���ÿ��ʱ������һ�����
        /*
        ÿ�μ���һ�����ݿ�16���ֽڣ���ȫ����Ҫ����
            s0 = GETU32(in     ) ^ rk[0];
            s1 = GETU32(in +  4) ^ rk[1];
            s2 = GETU32(in +  8) ^ rk[2];
            s3 = GETU32(in + 12) ^ rk[3];
        */
        unsigned char *p_in = 0;
        unsigned char *p_out = 0;
        unsigned char data[16] = { 0 };
        for (int i = 0; i < in_size; i += 16)
        {
            p_in = (unsigned char *)in + i;
            p_out = out + i;
            enc_byte += 16;
            AES_decrypt(p_in, p_out, &aes_);
        }
        return enc_byte;
    }
private:
    AES_KEY aes_;
    bool is_set_decode = false;
    bool is_set_encode = false;

};
XAES* XAES::Create()
{
    return new CXAES();
}