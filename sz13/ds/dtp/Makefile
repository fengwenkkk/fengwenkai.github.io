# Makefile

TOP_DIR = $(shell pwd)/..

include $(TOP_DIR)/Rules.mak
LIBNAME=$(TOP_DIR)/lib/dtp.a

all : $(LIBNAME)

EXTRA_CFLAGS := -I. -Icommon -Itcp_client -Itcp_server -Iudp_client -Iudp_server   
EXTRA_LDFLAGS :=

CSRCS :=
CPPSRCS := \
	common/dtp_func.cpp \
	common/DTPClient.cpp \
	common/DTPServer.cpp \
	tcp_client/dtp_tcp_client.cpp \
	tcp_client/dtp_tcp_client_send_thread.cpp \
	tcp_client/dtp_tcp_client_msg_handle.cpp \
	tcp_server/dtp_tcp_server.cpp \
	tcp_server/dtp_tcp_server_msg_handle.cpp \
	tcp_server/dtp_tcp_server_send_thread.cpp \
	udp_client/dtp_udp_client.cpp \
	udp_client/dtp_udp_client_send_thread.cpp \
	udp_client/dtp_udp_client_msg_handle.cpp \
	udp_server/dtp_udp_server.cpp \
	udp_server/dtp_udp_server_msg_handle.cpp \
	udp_server/dtp_udp_server_send_thread.cpp

clean-extra :
	- $(RM) $(LIBNAME)
	- $(RM) */*.o

include $(TOP_DIR)/Makerules
