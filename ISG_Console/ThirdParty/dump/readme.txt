�ȸ�breakpad��qt wrapper�������ڳ������ʱ����΢��minidump�ļ�����׷��ÿ���̵߳Ķ�ջ��Ϣ��

����ʹ�÷������£�

1.���ɱ���dmp�ļ���Ҫ��Qt����pro�ļ����������ļ�

include(../common/dump/CrashManager.pri)

2.main.cpp������������Ӵ���ָ��dmp�ļ�����·��Ϊ��������Ŀ¼

#include "../common/dump/CrashHandler/CrashHandler.h"
CrashManager::CrashHandler::instance()->Init(QApplication::applicationDirPath());

3.����dmp�ļ��Ĺ��߳�����Ҫȥsensor\common\dump\breakpadĿ¼�ֶ�����

./configure CXXFLAGS=-std=c++0x
make

4.�л�����������Ŀ¼��ִ������������ɷ��ű����ű��ļ�������ͳ�������һ�£�

cd /mnt/hgfs/sensor/sensor_pressure
./build.sh board
./../common/dump/breakpad/src/tools/linux/dump_syms/dump_syms ./sensor_pressure > sensor_pressure.sym

5.�鿴���ű��һ�У�����ʮ�������ַ�����CE7C0368C125E850C8C983389921BC360�����ڴ�������Ŀ¼

head -n1 sensor_pressure.sym

6.��������Ŀ¼�������ű�Ž�ȥ

mkdir -p ./symbols/sensor_pressure/CE7C0368C125E850C8C983389921BC360
mv sensor_pressure.sym ./symbols/sensor_pressure/CE7C0368C125E850C8C983389921BC360

7.�ڿͻ������£�������dump����С���Ӳ�����dmp�ļ�������ubuntu��ǰĿ¼(/mnt/hgfs/sensor/sensor_pressure)����Ϸ���Ŀ¼�����ɱ�����ջ��־

./../common/dump/breakpad/src/processor/minidump_stackwalk 37d8b788-29d9-3739-6253ede6-7408a671.dmp ./symbols > crash.log

8.��crash.log����ջ��Ϣ����˵����release�汾��������־�ļ��еĺ�����������Դ���벻һ�£������ܿ����кţ�

sensor_pressure!crash() [Main.cpp : 64 + 0x3]
���뵥Ԫ        |        |          |
                ������   |          |
                         �����ļ��� |
                                    �к�

9.�汾����ʱ��������ǰ׼�����ű�ֻ��Ҫ���ͻ�����������Ŀ�ִ���ļ��Ͷ�Ӧ��dmp�ļ������������ܽ��д���λ