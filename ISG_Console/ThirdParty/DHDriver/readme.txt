�ļ���Դ˵����
	��������뻷����Դ
	win��
		��װ�����ص�ַhttp://download.huaraytech.com/pub/sdk/Ver2.2.3/Windows/Base_ver/
		
		������ɺ󣬽��а�װ��
		�Ӱ�װ·����\DahuaTech\MV Viewer\Development\Samples\QT\ShowQImage����depends��include������
	linux��
		��http://download.huaraytech.com/pub/sdk/Ver2.2.3/Linux/x86/ ����linux�Ĵ�������װ��
		��linux������ʹ��rootȨ�ޣ�ִ��"bash MVviewer_Ver2.2.3_Linux_x86_Build20200323.run" ���������������İ�װ
		��װ��ɺ�ʹ��rootȨ�� ִ�� " ln -s libMVSDK.so.2.1.0.108758 libMVSDKmd.so " ���ɱ�������Ҫ���ļ�

�汾���뷽����
	win:
		��qt��������ӵ� INCLUDEPATH = ThirdParty/DHDriver/inc
		��qt��������ӵ� LIBS += -L$$PWD/ThirdParty/DHDriver/lib/x64/vs2013shared/ -lMVSDKmd
    		                 LIBS += -L$$PWD/ThirdParty/DHDriver/lib/x64/vs2013shared/ -lImageConvert
		�ڱ���ǰ �޸�QT���̵ı����׼�Ϊ MSVC 2017 �İ汾
		������������ɳɹ�������д������windows�����
	linux:
		
		�޸�qt�����ļ��� INCLUDEPATH = /opt/DahuaTech/MVviewer/include
		�޸�qt�����ļ��� LIBS += -L/opt/DahuaTech/MVviewer/lib -lMVSDKmd
    				 LIBS += -L/opt/DahuaTech/MVviewer/lib -lImageConvert
		Qt�ı����׼�ѡ��Ĭ�ϵ�gcc64 �ȿɱ�������linux��ĳ���

