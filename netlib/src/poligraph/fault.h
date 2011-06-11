#ifndef _FAULT_
#define _FAULT_

#define enable_link(p)	(link_state(p)=TRUE)
#define disable_link(p)	(link_state(p)=FALSE, link_capacity(p)=0, link_broken_capacity(p)=0)

#endif
