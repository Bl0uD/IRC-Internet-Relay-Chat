
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include "bircd.h"

void	client_read(t_env *e, int cs)
{
  int	r;
  int	i;

  r = recv(cs, e->_Fds[cs].buf_read, BUF_SIZE, 0);
  if (r <= 0)
    {
      close(cs);
      clean_fd(&e->_Fds[cs]);
      printf("client #%d gone away\n", cs);
    }
  else
    {
      i = 0;
      while (i < e->maxfd)
	{
	  if ((e->_Fds[i].type == FD_CLIENT) &&
	      (i != cs))
	    send(i, e->_Fds[cs].buf_read, r, 0);
	  i++;
	}
    }
}
