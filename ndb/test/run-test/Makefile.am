
testdir=$(prefix)/mysql-test/ndb

include $(top_srcdir)/ndb/config/common.mk.am
include $(top_srcdir)/ndb/config/type_util.mk.am
include $(top_srcdir)/ndb/config/type_mgmapiclient.mk.am

test_PROGRAMS = atrt
test_DATA=daily-basic-tests.txt daily-devel-tests.txt 16node-tests.txt \
          conf-daily-basic-ndbmaster.txt \
          conf-daily-basic-shark.txt \
          conf-daily-devel-ndbmaster.txt \
          conf-daily-sql-ndbmaster.txt \
          conf-daily-basic-dl145a.txt
test_SCRIPTS=atrt-analyze-result.sh atrt-gather-result.sh atrt-setup.sh \
          atrt-clear-result.sh make-config.sh make-index.sh make-html-reports.sh

atrt_SOURCES = main.cpp run-test.hpp
INCLUDES_LOC = -I$(top_srcdir)/ndb/test/include
LDADD_LOC = $(top_builddir)/ndb/test/src/libNDBT.a \
	    $(top_builddir)/ndb/src/libndbclient.la \
            $(top_builddir)/dbug/libdbug.a \
            $(top_builddir)/mysys/libmysys.a \
            $(top_builddir)/strings/libmystrings.a @NDB_SCI_LIBS@

wrappersdir=$(prefix)/bin
wrappers_SCRIPTS=atrt-testBackup atrt-mysql-test-run

EXTRA_DIST = $(test_DATA) $(test_SCRIPTS) $(wrappers_SCRIPTS) README.ATRT

# Don't update the files from bitkeeper
%::SCCS/s.%

windoze-dsp:
