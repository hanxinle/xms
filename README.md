
###  XMS ΢����ϵͳ���� API ���أ���־�������÷���ע����񣬷���������ģ� ͨ����� SDK�����ݿ��д SDK�� ���� SDK ���ģ��������ģ��Ӧ�߱����ж�����Ⱥ����������� XMS ϵͳӦ�ṩ C/S ��ϵͳ������棬�������ù�������״̬��ء���־��ѯ���ܡ� 

# ��Ŀ������Ա�͹���
## �Ĳܿ� 

<table border=1>
<tr>
<th>name</th>
<th>����</th>
<th>����</th>
</tr>
<tr>
<td>
�Ĳܿ�
</td>
<td>
1 ��Ŀ��ά<br>
2 XPlatformͨ�ſ⿪��
</td>
<td>
xiacaojun@qq.com
</td>
</tr>
</table>



# Windows���뻷��׼��
��װVS2017�����汾��QT5.9�汾
# Linux���뻷��׼��
ubuntu 18.04.02 x64
##?�����Ĺ���
    apt-get?install?perl?g++?make?automake?libtool?unzip git
# ��������밲װ
## zlib��protobuf��libevent��������ѹ����
    tar -xvf zlib-1.2.11.tar.gz
    cd zlib-1.2.11/
    ./configure
    make -j32
    make install
    # ��װ�� /usr/local/include/ /usr/local/lib Ŀ¼��

## openssl ��libevent����������ȫ���ܣ�
    tar -xvf openssl-1.1.1.tar.gz
    cd openssl-1.1.1/
    ./config
    make -j32
    make install
    # openssl ������ /usr/local/bin
    #���ð�װ�� /usr/local/ssl 
    #ͷ�ļ�/usr/local/include/openssl
    #so���ļ�/usr/local/lib
## protobuf��ͨ��Э�飩
    unzip protobuf-all-3.8.0.zip
    cd protobuf-3.8.0/
    ./configure
    make -j32
    make install
    #��װ�� /usr/local/include/google/protobuf 
    # protoc /usr/local/bin
    # so���ļ� /usr/local/lib
## libevent ������ͨ�ţ�
    unzip libevent-master.zip
    ./autogen.sh
    ./configure
    make -j32
    make install
    #��װ�� /usr/local/lib /usr/local/include


# XMS ϵͳ��װ

## ��װ���ݿ��������Linux��
### ��װ���ݿ�ͻ��˿�
    apt-get install libmysqlclient-dev
### ��װ���ݿ�����
    sudo apt-get install mysql-server
### �����û�������
    /etc/mysql/debian.cnf�ļ���������ļ�����ϵͳĬ�ϸ����Ƿ�����û���������
    mysql -u debian-sys-maint -p 
    set password for 'root'@'localhost' = password('123456')
## LXMysql �ⰲװ
    apt-get install libmysqlclient-dev
    cd /root/xms/src/LXMysql
    make -j32
    make install
    # ��װ�� /usr/lib/libLXMysql.so
## XPlatformͨ�ſⰲװ
    cd ../xplatform
    # ����proto��Ӧ��c++����
    make proto  
    make -j32
    make install
    # ��װ�� /usr/lib/libxcom.so
## XRCע�����İ�װ
����XPlatform�����һ�����XLOG��־����
ע�����ķ���˰�װ
    cd ../register_server
    make -j32
    make install
## ע�����Ŀͻ���
    cd ../register_client
    make -j32
    make install
## XLOG��־���İ�װ
    #����XPlatform��LXMysql register_client ������XRCע��΢����
    cd ../xlog/
    make -j32
    make install
## XCC�������İ�װ
    # ����LXMysql xplatform register_client ������XRCע��΢����
### ��������΢����
    cd ../config_server
    make -j32
    make install
### �������Ŀͻ���
    cd ../config_client
    make -j32
    make install
## XAUTH ��Ȩ���İ�װ
    # ����LXMysql xplatform register_client 
    # ͨ��register_client ����XRCע��΢����
    # ͨ�� config_client ��ȡ����
    cd ../xauth
    # ����xauth΢����
    make -j32  
    # ����xauth�ͻ���
    make libxauth.so
    make install
## XAG���ذ�װ
    # ����xplatform register_client config_client
    # ͨ��register_client��ȡȫ������΢�����б�
    # ͨ�� config_client ��ȡ��������
    make -j32
    make install