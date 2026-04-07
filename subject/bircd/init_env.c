
#include <stdlib.h>
#include <sys/resource.h>
#include "bircd.h"

void		init_env(t_env *e)
{
  int		i;
  struct rlimit	rlp;

  X(-1, getrlimit(RLIMIT_NOFILE, &rlp), "getrlimit");
  e->maxfd = rlp.rlim_cur;
  e->_Fds = (t_fd*)Xv(NULL, malloc(sizeof(*e->_Fds) * e->maxfd), "malloc");
  i = 0;
  while (i < e->maxfd)
    {
      clean_fd(&e->_Fds[i]);
      i++;
    }
}
