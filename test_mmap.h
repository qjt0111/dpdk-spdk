//  pcap.h  
//  pcaptest  
    //  
    //  Created by ly on 18-5-25.  
 
       
#ifndef pcaptest_pcap_h  
#define pcaptest_pcap_h 
 
//#include "comdef.h" 
#include <stdio.h>
//#include "pcap.h"
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include<sys/time.h>
#define MAGIC 0xa1b2c3d4
//#define MAJOR 2
//#define MINOR 4
       
typedef unsigned int  bpf_u_int32;  
typedef unsigned short  u_short;  
typedef int bpf_int32;  
       
    /* 
     Pcap�ļ�ͷ24B���ֶ�˵���� 
     Magic��4B��0x1A 2B 3C 4D:������ʾ�ļ��Ŀ�ʼ 
     Major��2B��0x02 00:��ǰ�ļ���Ҫ�İ汾��      
     Minor��2B��0x04 00��ǰ�ļ���Ҫ�İ汾�� 
     ThisZone��4B���صı�׼ʱ�䣻ȫ�� 
     SigFigs��4Bʱ����ľ��ȣ�ȫ�� 
     SnapLen��4B���Ĵ洢����     
     LinkType��4B��·���� 
     �������ͣ� 
     0            BSD loopback devices, except for later OpenBSD 
     1            Ethernet, and Linux loopback devices 
     6            802.5 Token Ring 
     7            ARCnet 
     8            SLIP 
     9            PPP 
     */  
typedef struct pcap_file_header {  
    bpf_u_int32 magic;  
    u_short major;  
    u_short minor;  
    bpf_int32 thiszone;      
    bpf_u_int32 sigfigs;     
    bpf_u_int32 snaplen;     
    bpf_u_int32 linktype;    
}pcap_file_header;  
       
    /* 
     Packet��ͷ��Packet������� 
     �ֶ�˵���� 
     Timestamp��ʱ�����λ����ȷ��seconds      
     Timestamp��ʱ�����λ����ȷ��microseconds 
     Caplen����ǰ�������ĳ��ȣ���ץȡ��������֡���ȣ��ɴ˿��Եõ���һ������֡��λ�á� 
     Len���������ݳ��ȣ�������ʵ������֡�ĳ��ȣ�һ�㲻����caplen����������º�Caplen��ֵ��ȡ� 
     Packet���ݣ��� Packet��ͨ��������·�������֡���������ݣ����Ⱦ���Caplen��������ȵĺ��棬���ǵ�ǰPCAP�ļ��д�ŵ���һ��Packet���ݰ���Ҳ�� ��˵��PCAP�ļ����沢û�й涨�����Packet���ݰ�֮����ʲô����ַ�������һ���������ļ��е���ʼλ�á�������Ҫ����һ��Packet��ȷ���� 
     */  
       
typedef struct  timestamp{  
    bpf_u_int32 timestamp_s;  
    bpf_u_int32 timestamp_ms;  
}timestamp;  
       
typedef struct pcap_header{  
    timestamp ts;  
    bpf_u_int32 capture_len;  
    bpf_u_int32 len;  
       
}pcap_header;  
       
       
 
//void encode_pcap(struct rte_mbuf *pktbuf);
      




 
int write_file_header(FILE *fd , pcap_file_header * pcap_file_hdr)
{
    int ret =0;
    if(fd <0 )
	return -1;
    ret = fwrite(pcap_file_hdr,sizeof(pcap_file_header),1,fd);
    if (ret != 1 )
	return -1;
    return 0;
}
 
int write_header(FILE *fd ,pcap_header * pcap_hdr)
{
    int ret =0;
    if(fd < 0 )
	return -1;
    ret = fwrite(pcap_hdr,sizeof(pcap_header),1,fd);
    if(ret != 1)
	return -1;
    return 0;
}
 
 
int write_pbuf(FILE *fd ,struct rte_mbuf *pktbuf)
{
    int ret =0;
    if (fd < 0 )
		return -1;
	char* hash_pkt = rte_pktmbuf_mtod(pktbuf,char*);
    ret = fwrite(hash_pkt,pktbuf->data_len,1,fd);
    if (ret != 1)
		return -1;
    return 0;
}


char* pos=NULL;
int f_len=0;



void encode_pcap(struct rte_mbuf *pktbuf)
{
    
  //  printf("into endecode \n");
    u_short major =2;
    u_short minor =4;
    //char *pbuf = (char*)pktbuf;
    char* pbuf = rte_pktmbuf_mtod(pktbuf,char*);
    uint16_t len =pktbuf->data_len;
    pcap_file_header pcap_file_hdr;
    timestamp pcap_timemp;
    pcap_header pcap_hdr;
 
    // printf("[pcap_file_hdr]=%d\n",sizeof(pcap_file_hdr));
    // printf("[pcap_hdr] = %d \n",sizeof(pcap_hdr));
    // printf("[pcap_file_header]  = %d  \n",sizeof(pcap_file_header));
    // printf("[pcap_header]  =  %d  \n",sizeof(pcap_header));
 
 
//��ʼ��pcapͷ
   // printf("----struct--\n");
    pcap_file_hdr.magic = 0xa1b2c3d4;
    //printf("magic\n");
    pcap_file_hdr.major = major;
    pcap_file_hdr.minor = minor;
    pcap_file_hdr.thiszone = 0;
    pcap_file_hdr.sigfigs  = 0;
    //printf("snaplen\n");
    pcap_file_hdr.snaplen  = 65535;
    //printf("pcap_file_hdr\n");
    pcap_file_hdr.linktype =1;
    
    struct timeval tv;
    gettimeofday(&tv,NULL);
    
    pcap_timemp.timestamp_s =0;
    pcap_timemp.timestamp_ms= 0;
 
    pcap_hdr.capture_len = (uint32_t)pktbuf->pkt_len;
    pcap_hdr.len = (uint32_t)pktbuf->pkt_len;
    //pcap_hdr.ts = pcap_timemp;
    int ret =0;
 
    // ret = access("encode1.pcap",0);//ֻд��һ���ļ��������ж��ļ��Ƿ����
    // if(ret == -1)
    // {//�����ڴ���������д��pcap�ļ�ͷ����pcapͷ�����б��ģ�tcp\ip ͷ�����ݣ�
    //     FILE *fd;
    //     int ret =0;
	// fd = fopen("encode1.pcap","wb+");
	// if (fd == NULL )
	// {
	//     printf("w+:can't open the file\n");
	//     return;
	// }

    // int f_ret = ftruncate((int)fd, sizeof(pcap_file_hdr)+sizeof(pcap_hdr)+pktbuf->pkt_len);
    // char* addr = (char *)mmap(NULL, sizeof(pcap_file_hdr)+sizeof(pcap_hdr)+pktbuf->pkt_len, PROT_READ | PROT_WRITE, MAP_SHARED, (int)fd, 0);

    // memcpy(addr,&pcap_file_hdr,sizeof(pcap_file_hdr));
    // memcpy(addr+sizeof(pcap_file_hdr),&pcap_hdr,sizeof(pcap_hdr));

    // memcpy(addr+sizeof(pcap_file_hdr)+sizeof(pcap_hdr),pbuf,pktbuf->pkt_len);
    // fclose(fd);
    // munmap((void *)addr, 0);
    // //     ret = write_file_header(fd,&pcap_file_hdr);
	// // if(ret == -1 )
	// // {
	// //     printf("write file header error!\n");
	// //     return;
	// // }
	// // fseek(fd,0,SEEK_END);
	// // ret = write_header(fd,&pcap_hdr);
	// // if(ret == -1 )
	// // {
	// //     printf("write header error!\n");
	// //     return;
	// // }
	// // fseek(fd,0,SEEK_END);
	// // ret = write_pbuf(fd,pktbuf);
	// // if (ret == -1)
	// // {
	// //     printf("write pbuf error!\n");
	// //     return;
	// // }
    // //     fclose(fd);
    // //     return;
    // }
    int fd_pcap;//����ļ��Ѵ��ڣ�ֱ�����ļ�ĩβд�룬pcapͷ������
    int ret_a =0;
    fd_pcap = open("encode1.pcap",O_RDWR | O_CREAT, 0644);
    if (fd_pcap == -1)
    {
        printf("a+:can't no open file !\n");
        return;
    }
    struct stat st;
    ret_a = fstat(fd_pcap,&st);
    if(ret_a==-1)
    {
        printf("��ȡ�ļ���Сʧ��:%m\n");
        close(fd_pcap);
        return; 
    }
    int length = st.st_size;//lseek(fd_pcap,0,SEEK_END);
    pos+=f_len;
    if(length==0)
    {
        int f_ret = ftruncate(fd_pcap, sizeof(pcap_file_hdr)+sizeof(pcap_hdr)+pktbuf->pkt_len);
        printf("first\n");
        if(f_ret)
        {
            printf("ftruncate\n");
        }

        char* addr = (char *)mmap(pos, sizeof(pcap_file_hdr)+sizeof(pcap_hdr)+pktbuf->pkt_len, PROT_WRITE, MAP_SHARED, fd_pcap, 0);

        printf("first222\n");
        memcpy(addr,&pcap_file_hdr,sizeof(pcap_file_hdr));
        printf("-----%x\n",*(uint32_t*)addr);
        memcpy(addr+sizeof(pcap_file_hdr),&pcap_hdr,sizeof(pcap_hdr));
        printf("%u\n",sizeof(pcap_file_hdr));
        printf("%x\n",*(uint32_t*)(addr+sizeof(pcap_file_hdr)));
        printf("%u\n",sizeof(pcap_file_hdr)+sizeof(pcap_hdr));
        memcpy(addr+sizeof(pcap_file_hdr)+sizeof(pcap_hdr),pbuf,pktbuf->pkt_len);
        //printf("%s",addr+sizeof(pcap_file_hdr)+sizeof(pcap_hdr));
        f_len=sizeof(pcap_file_hdr)+sizeof(pcap_hdr)+pktbuf->pkt_len;
        printf("%u---%u----%u\n",pktbuf->pkt_len,pktbuf->data_len,sizeof(pcap_file_hdr)+sizeof(pcap_hdr)+pktbuf->pkt_len);
        printf("first333\n");
        
        munmap((void *)addr, sizeof(pcap_file_hdr)+sizeof(pcap_hdr)+pktbuf->pkt_len);
        close(fd_pcap);
    }else
    {
        char buf[128];
        //fgets(buf,128,fd_pcap);
        //buf = fd_pcap.read();
        int ret_a = read(fd_pcap,buf,128);
        //if(ret_a)
        // if(length<1024)
        // {
            printf("-----%x\n",*(uint32_t*)buf);
        
        
        
        int f_ret = ftruncate(fd_pcap, length+sizeof(pcap_hdr)+pktbuf->pkt_len);
        if(f_ret)
        {
            printf("ftruncate\n");
        }
        char* addr = (char *)mmap(NULL, length+sizeof(pcap_hdr)+pktbuf->pkt_len, PROT_WRITE, MAP_SHARED, fd_pcap, 0);

        // //memcpy(addr+length,&pcap_file_hdr,sizeof(pcap_file_hdr));
        memcpy(addr+length,&pcap_hdr,sizeof(pcap_hdr));
        memcpy(addr+length+sizeof(pcap_hdr),pbuf,pktbuf->data_len);
        //pos-=f_len;
        f_len =sizeof(pcap_hdr)+pktbuf->pkt_len;
        //close(fd_pcap);
        munmap((void *)addr, length+sizeof(pcap_hdr)+pktbuf->pkt_len);
       // }
        close(fd_pcap);
    }
    


    // ret_a = write_header(fd_pcap,&pcap_hdr);
    // if(ret_a == -1)
    // {
	// printf("write header error! \n");
	// return;
    // }
    // fseek(fd_pcap,0,SEEK_END);
    // ret_a =write_pbuf(fd_pcap,pktbuf);
    // if (ret_a == -1)
    // {
	// printf("write pbuf error! \n");
	// return;
    // }
    // fclose(fd_pcap);
   // printf("111end\n");
    return;
      
}
    #endif  