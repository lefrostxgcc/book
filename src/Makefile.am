bin_PROGRAMS = book
book_SOURCES=main.c ch_sqlite.c
book_CPPFLAGS=-I$(top_srcdir)/include @PKGGTK_CFLAGS@ @SQLITE3_CFLAGS@ \
$(AM_CPPFLAGS) -DDATA_PATH=\"$(pkgdatadir)\"
book_LDADD=@PKGGTK_LIBS@ @SQLITE3_LIBS@
