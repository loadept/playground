package main

/*
#cgo CFLAGS: -I.
#include "tcp.h"
*/
import "C"
import (
	"log/slog"
	"os"
	"os/signal"
	"syscall"
)

func main() {
	addr := C.CString("127.0.0.1")
	port := C.int(8080)

	sockFD := C.tcp_server(addr, port)
	if sockFD < 0 {
		slog.Error("Error to listen on TCP socker")
		os.Exit(1)
	}

	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, os.Interrupt, syscall.SIGTERM)
	defer signal.Stop(sigChan)

	go func() {
		C.accept_conns(sockFD)
	}()

	<-sigChan
	C.stop_listening(sockFD)
}
