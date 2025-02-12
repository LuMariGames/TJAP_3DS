#include "soc_common.h"
#include <errno.h>
#include <sys/socket.h>
#include <3ds/ipc.h>

ssize_t socuipc_cmd9(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
	int ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	u32 tmp_addrlen = 0;
	u8 tmpaddr[ADDR_STORAGE_LEN];

	memset(tmpaddr, 0, ADDR_STORAGE_LEN);

	if(dest_addr) {
		if(dest_addr->sa_family == AF_INET)
			tmp_addrlen = 8;
		else
			tmp_addrlen = ADDR_STORAGE_LEN;

		if(addrlen < tmp_addrlen) {
			errno = EINVAL;
			return -1;
		}

		tmpaddr[0] = tmp_addrlen;
		tmpaddr[1] = dest_addr->sa_family;
		memcpy(&tmpaddr[2], &dest_addr->sa_data, tmp_addrlen-2);
	}

	cmdbuf[0] = IPC_MakeHeader(0x9,4,6); // 0x90106
	cmdbuf[1] = (u32)sockfd;
	cmdbuf[2] = (u32)len;
	cmdbuf[3] = (u32)flags;
	cmdbuf[4] = (u32)tmp_addrlen;
	cmdbuf[5] = IPC_Desc_CurProcessId();
	cmdbuf[7] = IPC_Desc_StaticBuffer(tmp_addrlen,1);
	cmdbuf[8] = (u32)tmpaddr;
	cmdbuf[9] = IPC_Desc_Buffer(len,IPC_BUFFER_R);
	cmdbuf[10] = (u32)buf;

	ret = svcSendSyncRequest(SOCU_handle);
	if(ret != 0) {
		errno = SYNC_ERROR;
		return ret;
	}

	ret = (int)cmdbuf[1];
	if(ret == 0)
		ret = _net_convert_error(cmdbuf[2]);

	if(ret < 0) {
		errno = -ret;
		return -1;
	}

	return ret;
}

ssize_t socuipc_cmda(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
	int ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	u32 tmp_addrlen = 0;
	u8 tmpaddr[ADDR_STORAGE_LEN];

	memset(tmpaddr, 0, ADDR_STORAGE_LEN);

	if(dest_addr) {
		if(dest_addr->sa_family == AF_INET)
			tmp_addrlen = 8;
		else
			tmp_addrlen = ADDR_STORAGE_LEN;

		if(addrlen < tmp_addrlen) {
			errno = EINVAL;
			return -1;
		}

		tmpaddr[0] = tmp_addrlen;
		tmpaddr[1] = dest_addr->sa_family;
		memcpy(&tmpaddr[2], &dest_addr->sa_data, tmp_addrlen-2);
	}

	cmdbuf[0] = IPC_MakeHeader(0xA,4,6); // 0xA0106
	cmdbuf[1] = (u32)sockfd;
	cmdbuf[2] = (u32)len;
	cmdbuf[3] = (u32)flags;
	cmdbuf[4] = (u32)tmp_addrlen;
	cmdbuf[5] = IPC_Desc_CurProcessId();
	cmdbuf[7] = IPC_Desc_StaticBuffer(len,2);
	cmdbuf[8] = (u32)buf;
	cmdbuf[9] = IPC_Desc_StaticBuffer(tmp_addrlen,1);
	cmdbuf[10] = (u32)tmpaddr;

	ret = svcSendSyncRequest(SOCU_handle);
	if(ret != 0) {
		errno = SYNC_ERROR;
		return ret;
	}

	ret = (int)cmdbuf[1];
	if(ret == 0)
		ret = _net_convert_error(cmdbuf[2]);

	if(ret < 0) {
		errno = -ret;
		return -1;
	}

	return ret;
}

ssize_t soc_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
	if(len < 0x2000)
		return socuipc_cmda(sockfd, buf, len, flags, dest_addr, addrlen);
	return socuipc_cmd9(sockfd, buf, len, flags, dest_addr, addrlen);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
	sockfd = soc_get_fd(sockfd);
	if(sockfd < 0) {
		errno = -sockfd;
		return -1;
	}

	return soc_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
}
