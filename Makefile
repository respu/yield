all: yield yield.aio yield.aio_test yield.fs yield.fs.poll yield.fs.poll_test yield.fs_test yield.http yield.http.server yield.http.server_test yield.http_test yield.i18n yield.i18n_test yield.poll yield.poll_test yield.queue yield.queue_test yield.sockets yield.sockets.aio yield.sockets.aio_test yield.sockets.poll yield.sockets.poll_test yield.sockets_test yield.stage yield.stage_test yield.thread yield.thread_test yield.uri yield.uri_test yield_test

clean:
	$(MAKE) -C proj/yield -f yield.Makefile clean
	$(MAKE) -C proj/yield -f yield_test.Makefile clean
	$(MAKE) -C proj/yield/aio -f yield.aio.Makefile clean
	$(MAKE) -C proj/yield/aio -f yield.aio_test.Makefile clean
	$(MAKE) -C proj/yield/fs -f yield.fs.Makefile clean
	$(MAKE) -C proj/yield/fs -f yield.fs_test.Makefile clean
	$(MAKE) -C proj/yield/fs/poll -f yield.fs.poll.Makefile clean
	$(MAKE) -C proj/yield/fs/poll -f yield.fs.poll_test.Makefile clean
	$(MAKE) -C proj/yield/http -f yield.http.Makefile clean
	$(MAKE) -C proj/yield/http -f yield.http_test.Makefile clean
	$(MAKE) -C proj/yield/http/server -f yield.http.server.Makefile clean
	$(MAKE) -C proj/yield/http/server -f yield.http.server_test.Makefile clean
	$(MAKE) -C proj/yield/i18n -f yield.i18n.Makefile clean
	$(MAKE) -C proj/yield/i18n -f yield.i18n_test.Makefile clean
	$(MAKE) -C proj/yield/poll -f yield.poll.Makefile clean
	$(MAKE) -C proj/yield/poll -f yield.poll_test.Makefile clean
	$(MAKE) -C proj/yield/queue -f yield.queue.Makefile clean
	$(MAKE) -C proj/yield/queue -f yield.queue_test.Makefile clean
	$(MAKE) -C proj/yield/sockets -f yield.sockets.Makefile clean
	$(MAKE) -C proj/yield/sockets -f yield.sockets_test.Makefile clean
	$(MAKE) -C proj/yield/sockets/aio -f yield.sockets.aio.Makefile clean
	$(MAKE) -C proj/yield/sockets/aio -f yield.sockets.aio_test.Makefile clean
	$(MAKE) -C proj/yield/sockets/poll -f yield.sockets.poll.Makefile clean
	$(MAKE) -C proj/yield/sockets/poll -f yield.sockets.poll_test.Makefile clean
	$(MAKE) -C proj/yield/stage -f yield.stage.Makefile clean
	$(MAKE) -C proj/yield/stage -f yield.stage_test.Makefile clean
	$(MAKE) -C proj/yield/thread -f yield.thread.Makefile clean
	$(MAKE) -C proj/yield/thread -f yield.thread_test.Makefile clean
	$(MAKE) -C proj/yield/uri -f yield.uri.Makefile clean
	$(MAKE) -C proj/yield/uri -f yield.uri_test.Makefile clean

depclean:
	$(MAKE) -C proj/yield -f yield.Makefile depclean
	$(MAKE) -C proj/yield -f yield_test.Makefile depclean
	$(MAKE) -C proj/yield/aio -f yield.aio.Makefile depclean
	$(MAKE) -C proj/yield/aio -f yield.aio_test.Makefile depclean
	$(MAKE) -C proj/yield/fs -f yield.fs.Makefile depclean
	$(MAKE) -C proj/yield/fs -f yield.fs_test.Makefile depclean
	$(MAKE) -C proj/yield/fs/poll -f yield.fs.poll.Makefile depclean
	$(MAKE) -C proj/yield/fs/poll -f yield.fs.poll_test.Makefile depclean
	$(MAKE) -C proj/yield/http -f yield.http.Makefile depclean
	$(MAKE) -C proj/yield/http -f yield.http_test.Makefile depclean
	$(MAKE) -C proj/yield/http/server -f yield.http.server.Makefile depclean
	$(MAKE) -C proj/yield/http/server -f yield.http.server_test.Makefile depclean
	$(MAKE) -C proj/yield/i18n -f yield.i18n.Makefile depclean
	$(MAKE) -C proj/yield/i18n -f yield.i18n_test.Makefile depclean
	$(MAKE) -C proj/yield/poll -f yield.poll.Makefile depclean
	$(MAKE) -C proj/yield/poll -f yield.poll_test.Makefile depclean
	$(MAKE) -C proj/yield/queue -f yield.queue.Makefile depclean
	$(MAKE) -C proj/yield/queue -f yield.queue_test.Makefile depclean
	$(MAKE) -C proj/yield/sockets -f yield.sockets.Makefile depclean
	$(MAKE) -C proj/yield/sockets -f yield.sockets_test.Makefile depclean
	$(MAKE) -C proj/yield/sockets/aio -f yield.sockets.aio.Makefile depclean
	$(MAKE) -C proj/yield/sockets/aio -f yield.sockets.aio_test.Makefile depclean
	$(MAKE) -C proj/yield/sockets/poll -f yield.sockets.poll.Makefile depclean
	$(MAKE) -C proj/yield/sockets/poll -f yield.sockets.poll_test.Makefile depclean
	$(MAKE) -C proj/yield/stage -f yield.stage.Makefile depclean
	$(MAKE) -C proj/yield/stage -f yield.stage_test.Makefile depclean
	$(MAKE) -C proj/yield/thread -f yield.thread.Makefile depclean
	$(MAKE) -C proj/yield/thread -f yield.thread_test.Makefile depclean
	$(MAKE) -C proj/yield/uri -f yield.uri.Makefile depclean
	$(MAKE) -C proj/yield/uri -f yield.uri_test.Makefile depclean


yield.aio: yield
	$(MAKE) -C proj/yield/aio -f yield.aio.Makefile

yield.aio_test: yield.aio
	$(MAKE) -C proj/yield/aio -f yield.aio_test.Makefile

yield.fs.poll: yield.aio yield.fs
	$(MAKE) -C proj/yield/fs/poll -f yield.fs.poll.Makefile

yield.fs.poll_test: yield.fs.poll
	$(MAKE) -C proj/yield/fs/poll -f yield.fs.poll_test.Makefile

yield.fs: yield.i18n
	$(MAKE) -C proj/yield/fs -f yield.fs.Makefile

yield.fs_test: yield.fs
	$(MAKE) -C proj/yield/fs -f yield.fs_test.Makefile

yield.http.server: yield.fs yield.http yield.sockets.aio
	$(MAKE) -C proj/yield/http/server -f yield.http.server.Makefile

yield.http.server_test: yield.http.server
	$(MAKE) -C proj/yield/http/server -f yield.http.server_test.Makefile

yield.http: yield.uri
	$(MAKE) -C proj/yield/http -f yield.http.Makefile

yield.http_test: yield.http
	$(MAKE) -C proj/yield/http -f yield.http_test.Makefile

yield.i18n: yield
	$(MAKE) -C proj/yield/i18n -f yield.i18n.Makefile

yield.i18n_test: yield.i18n
	$(MAKE) -C proj/yield/i18n -f yield.i18n_test.Makefile

yield.poll: yield.queue
	$(MAKE) -C proj/yield/poll -f yield.poll.Makefile

yield.poll_test: yield.poll
	$(MAKE) -C proj/yield/poll -f yield.poll_test.Makefile

yield.queue: yield.thread
	$(MAKE) -C proj/yield/queue -f yield.queue.Makefile

yield.queue_test: yield.queue
	$(MAKE) -C proj/yield/queue -f yield.queue_test.Makefile

yield.sockets.aio: yield.aio yield.sockets.poll
	$(MAKE) -C proj/yield/sockets/aio -f yield.sockets.aio.Makefile

yield.sockets.aio_test: yield.fs yield.sockets.aio
	$(MAKE) -C proj/yield/sockets/aio -f yield.sockets.aio_test.Makefile

yield.sockets.poll: yield.poll yield.sockets
	$(MAKE) -C proj/yield/sockets/poll -f yield.sockets.poll.Makefile

yield.sockets.poll_test: yield.sockets.poll
	$(MAKE) -C proj/yield/sockets/poll -f yield.sockets.poll_test.Makefile

yield.sockets: yield
	$(MAKE) -C proj/yield/sockets -f yield.sockets.Makefile

yield.sockets_test: yield.fs yield.sockets
	$(MAKE) -C proj/yield/sockets -f yield.sockets_test.Makefile

yield.stage: yield.thread
	$(MAKE) -C proj/yield/stage -f yield.stage.Makefile

yield.stage_test: yield.stage
	$(MAKE) -C proj/yield/stage -f yield.stage_test.Makefile

yield.thread: yield
	$(MAKE) -C proj/yield/thread -f yield.thread.Makefile

yield.thread_test: yield.thread
	$(MAKE) -C proj/yield/thread -f yield.thread_test.Makefile

yield.uri: yield
	$(MAKE) -C proj/yield/uri -f yield.uri.Makefile

yield.uri_test: yield.uri
	$(MAKE) -C proj/yield/uri -f yield.uri_test.Makefile

yield: 
	$(MAKE) -C proj/yield -f yield.Makefile

yield_test: yield
	$(MAKE) -C proj/yield -f yield_test.Makefile

