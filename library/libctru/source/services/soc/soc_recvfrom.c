#include "soc_common.h"
#include <errno.h>
#include <sys/socket.h>
#include <3ds/ipc.h>

ssize_t socuipc_cmd7(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
	int ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	u32 tmp_addrlen = 0;
	u8 tmpaddr[ADDR_STORAGE_LEN];
	u32 saved_threadstorage[2];

	memset(tmpaddr, 0, ADDR_STORAGE_LEN);

	if(src_addr)
		tmp_addrlen = ADDR_STORAGE_LEN;

	cmdbuf[0] = IPC_MakeHeader(0x7,4,4); // 0x70104
	cmdbuf[1] = (u32)sockfd;
	cmdbuf[2] = (u32)len;
	cmdbuf[3] = (u32)flags;
	cmdbuf[4] = (u32)tmp_addrlen;
	cmdbuf[5] = IPC_Desc_CurProcessId();
	cmdbuf[7] = IPC_Desc_Buffer(len,IPC_BUFFER_W);
	cmdbuf[8] = (u32)buf;

	u32 * staticbufs = getThreadStaticBuffers();
	saved_threadstorage[0] = staticbufs[0];
	saved_threadstorage[1] = staticbufs[1];

	staticbufs[0] = IPC_Desc_StaticBuffer(tmp_addrlen,0);
	staticbufs[1] = (u32)tmpaddr;

	ret = svcSendSyncRequest(SOCU_handle);

	staticbufs[0] = saved_threadstorage[0];
	staticbufs[1] = saved_threadstorage[1];

	if(ret != 0) {
		errno = SYNC_ERROR;
		return -1;
	}

	ret = (int)cmdbuf[1];
	if(ret == 0)
		ret = _net_convert_error(cmdbuf[2]);

	if(ret < 0) {
		errno = -ret;
		return -1;
	}

	if(src_addr != NULL) {
		src_addr->sa_family = tmpaddr[1];

		socklen_t user_addrlen = tmpaddr[0];
		if(src_addr->sa_family == AF_INET)
			user_addrlen += 8;

		if(*addrlen > user_addrlen)
			*addrlen = user_addrlen;
		memcpy(src_addr->sa_data, &tmpaddr[2], *addrlen - sizeof(src_addr->sa_family));
	}

	return ret;
}

ssize_t socuipc_cmd8(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
	int ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	u32 tmp_addrlen = 0;
	u8 tmpaddr[ADDR_STORAGE_LEN];
	u32 saved_threadstorage[4];

	if(src_addr)
		tmp_addrlen = ADDR_STORAGE_LEN;

	memset(tmpaddr, 0, ADDR_STORAGE_LEN);

	cmdbuf[0] = 0x00080102;
	cmdbuf[1] = (u32)sockfd;
	cmdbuf[2] = (u32)len;
	cmdbuf[3] = (u32)flags;
	cmdbuf[4] = (u32)tmp_addrlen;
	cmdbuf[5] = 0x20;

	saved_threadstorage[0] = cmdbuf[0x100>>2];
	saved_threadstorage[1] = cmdbuf[0x104>>2];
	saved_threadstorage[2] = cmdbuf[0x108>>2];
	saved_threadstorage[3] = cmdbuf[0x10c>>2];

	cmdbuf[0x100>>2] = (((u32)len)<<14) | 2;
	cmdbuf[0x104>>2] = (u32)buf;
	cmdbuf[0x108>>2] = (tmp_addrlen<<14) | 2;
	cmdbuf[0x10c>>2] = (u32)tmpaddr;

	ret = svcSendSyncRequest(SOCU_handle);
	if(ret != 0) {
		errno = SYNC_ERROR;
		return ret;
	}

	cmdbuf[0x100>>2] = saved_threadstorage[0];
	cmdbuf[0x104>>2] = saved_threadstorage[1];
	cmdbuf[0x108>>2] = saved_threadstorage[2];
	cmdbuf[0x10c>>2] = saved_threadstorage[3];

	ret = (int)cmdbuf[1];
	if(ret == 0)
		ret = _net_convert_error(cmdbuf[2]);

	if(ret < 0) {
		errno = -ret;
		return -1;
	}

	if(src_addr != NULL) {
		src_addr->sa_family = tmpaddr[1];

		socklen_t user_addrlen = tmpaddr[0];
		if(src_addr->sa_family == AF_INET)
			user_addrlen += 8;

		if(*addrlen > user_addrlen)
			*addrlen = user_addrlen;
		memcpy(src_addr->sa_data, &tmpaddr[2], *addrlen - sizeof(src_addr->sa_family));
	}

	return ret;
}

ssize_t soc_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
	if(len < 0x2000)
		return socuipc_cmd8(sockfd, buf, len, flags, src_addr, addrlen);
	return socuipc_cmd7(sockfd, buf, len, flags, src_addr, addrlen);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
	sockfd = soc_get_fd(sockfd);
	if(sockfd < 0) {
		errno = -sockfd;
		return -1;
	}

	return soc_recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
}
