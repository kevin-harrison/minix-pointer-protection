#ifndef _CFM_PROTO_H
#define _CFM_PROTO_H

int main(int argc, char **argv);

int do_verify_hash(message *m_ptr);
int sef_cb_init_fresh(int type, sef_init_info_t *info);


#endif
