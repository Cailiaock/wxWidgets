/////////////////////////////////////////////////////////////////////////////
// Name:        stream.h
// Purpose:     "wxWindows stream" base classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WXSTREAM_H__
#define _WX_WXSTREAM_H__

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>
#include "wx/object.h"
#include "wx/string.h"
#include "wx/filefn.h"  // for off_t, wxInvalidOffset and wxSeekMode

class WXDLLEXPORT wxStreamBase;
class WXDLLEXPORT wxInputStream;
class WXDLLEXPORT wxOutputStream;

typedef wxInputStream& (*__wxInputManip)(wxInputStream&);
typedef wxOutputStream& (*__wxOutputManip)(wxOutputStream&);

wxOutputStream& WXDLLEXPORT wxEndL(wxOutputStream& o_stream);

// ---------------------------------------------------------------------------
// Stream buffer
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxStreamBuffer {
 public:
  typedef enum {
    read, write, read_write
  } BufMode;

  // -----------
  // ctor & dtor
  // -----------
  wxStreamBuffer(wxStreamBase& stream, BufMode mode);
  wxStreamBuffer(BufMode mode);
  wxStreamBuffer(const wxStreamBuffer& buf);
  ~wxStreamBuffer();

  // -----------
  // Filtered IO
  // -----------
  void Read(void *buffer, size_t size);
  void Write(const void *buffer, size_t size);
  bool WriteBack(const char *buffer, size_t size);
  bool WriteBack(char c);
  char GetChar();
  void PutChar(char c);
  off_t Tell() const;
  off_t Seek(off_t pos, wxSeekMode mode);

  // --------------
  // Buffer control
  // --------------
  void ResetBuffer();
  void SetBufferIO(char *buffer_start, char *buffer_end);
  void SetBufferIO(size_t bufsize);
  char *GetBufferStart() const { return m_buffer_start; }
  char *GetBufferEnd() const { return m_buffer_end; }
  char *GetBufferPos() const { return m_buffer_pos; }
  off_t GetIntPosition() const { return m_buffer_pos-m_buffer_start; }
  void SetIntPosition(off_t pos) { m_buffer_pos = m_buffer_start+pos; }
  size_t GetLastAccess() const { return m_buffer_end-m_buffer_start; }

  void Fixed(bool fixed) { m_fixed = fixed; }
  void Flushable(bool f) { m_flushable = f; }

  bool FlushBuffer();
  bool FillBuffer();
  size_t GetDataLeft() const;

 protected:
  char *AllocSpaceWBack(size_t needed_size);
  size_t GetWBack(char *buf, size_t bsize);

  void GetFromBuffer(void *buffer, size_t size);
  void PutToBuffer(const void *buffer, size_t size);

 protected:
  char *m_buffer_start, *m_buffer_end, *m_buffer_pos;
  size_t m_buffer_size;

  char *m_wback;
  size_t m_wbacksize, m_wbackcur;

  bool m_fixed, m_flushable;

  wxStreamBase *m_stream;
  BufMode m_mode;
  bool m_destroybuf;
};

// ---------------------------------------------------------------------------
// wxStream: base classes
// ---------------------------------------------------------------------------

typedef enum {
  wxStream_NOERROR,
  wxStream_EOF
} wxStreamError;

class WXDLLEXPORT wxStreamBase {
 public:
  wxStreamBase();
  virtual ~wxStreamBase();

  wxStreamError LastError() const { return m_lasterror; }
  virtual size_t StreamSize() const { return ~((size_t)0); }

 protected:
  friend class wxStreamBuffer;

  virtual size_t OnSysRead(void *buffer, size_t bufsize);
  virtual size_t OnSysWrite(const void *buffer, size_t bufsize);
  virtual off_t OnSysSeek(off_t seek, wxSeekMode mode);
  virtual off_t OnSysTell() const;

 protected:
  size_t m_lastcount;
  wxStreamError m_lasterror;
};

class WXDLLEXPORT wxInputStream: public wxStreamBase {
 public:
  wxInputStream();
  wxInputStream(wxStreamBuffer *sbuf);
  virtual ~wxInputStream();

  // IO functions
  virtual char Peek();
  char GetC();
  wxInputStream& Read(void *buffer, size_t size);
  wxInputStream& Read(wxOutputStream& stream_out);

  // Position functions
  off_t SeekI(off_t pos, wxSeekMode mode = wxFromStart);
  off_t TellI() const;

  // State functions
  wxStreamBuffer *InputStreamBuffer() { return m_i_streambuf; }
  size_t LastRead() { return wxStreamBase::m_lastcount; }

  // Operators
  wxInputStream& operator>>(wxOutputStream& out) { return Read(out); }
  wxInputStream& operator>>(wxString& line);
  wxInputStream& operator>>(char& c);
  wxInputStream& operator>>(short& i);
  wxInputStream& operator>>(int& i);
  wxInputStream& operator>>(long& i);
  wxInputStream& operator>>(double& i);
#if wxUSE_SERIAL
  wxInputStream& operator>>(wxObject *& obj);
#endif

  wxInputStream& operator>>(float& f) { double d; operator>>((double&)d); f = (float)d; return *this; }
  wxInputStream& operator>>(unsigned char& c) { return operator>>((char&)c); }
  wxInputStream& operator>>(unsigned short& i) { return operator>>((short&)i); }
  wxInputStream& operator>>(unsigned int& i) { return operator>>((int&)i); }
  wxInputStream& operator>>(unsigned long& i) { return operator>>((long&)i); }
  wxInputStream& operator>>( __wxInputManip func) { return func(*this); }

 protected:
  bool m_i_destroybuf;
  wxStreamBuffer *m_i_streambuf;
};

class WXDLLEXPORT wxOutputStream: public wxStreamBase {
 public:
  wxOutputStream();
  wxOutputStream(wxStreamBuffer *sbuf);
  virtual ~wxOutputStream();

  wxOutputStream& Write(const void *buffer, size_t size);
  wxOutputStream& Write(wxInputStream& stream_in);

  off_t SeekO(off_t pos, wxSeekMode mode = wxFromStart);
  off_t TellO() const;

  size_t LastWrite() const { return wxStreamBase::m_lastcount; }
  wxStreamBuffer *OutputStreamBuffer() { return m_o_streambuf; }

  void Sync();

  wxOutputStream& operator<<(wxInputStream& out) { return Write(out); }
  wxOutputStream& operator<<(const char *string);
  wxOutputStream& operator<<(wxString& string);
  wxOutputStream& operator<<(char c);
  wxOutputStream& operator<<(short i);
  wxOutputStream& operator<<(int i);
  wxOutputStream& operator<<(long i);
  wxOutputStream& operator<<(double f);
#if wxUSE_SERIAL
  wxOutputStream& operator<<(wxObject& obj);
#endif

  wxOutputStream& operator<<(float f) { return operator<<((double)f); }
  wxOutputStream& operator<<(unsigned char c) { return operator<<((char)c); }
  wxOutputStream& operator<<(unsigned short i) { return operator<<((short)i); }
  wxOutputStream& operator<<(unsigned int i) { return operator<<((int)i); }
  wxOutputStream& operator<<(unsigned long i) { return operator<<((long)i); }
  wxOutputStream& operator<<( __wxOutputManip func) { return func(*this); }

 protected:
  bool m_o_destroybuf;
  wxStreamBuffer *m_o_streambuf;
};

// ---------------------------------------------------------------------------
// "Filter" streams
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxFilterInputStream: public wxInputStream {
 public:
  wxFilterInputStream();
  wxFilterInputStream(wxInputStream& stream);
  ~wxFilterInputStream();

  char Peek() { return m_parent_i_stream->Peek(); }

  wxStreamError LastError() const { return m_parent_i_stream->LastError(); }
  size_t StreamSize() const { return m_parent_i_stream->StreamSize(); }

 protected:
  wxInputStream *m_parent_i_stream;
};

class WXDLLEXPORT wxFilterOutputStream: public wxOutputStream {
 public:
  wxFilterOutputStream();
  wxFilterOutputStream(wxOutputStream& stream);
  ~wxFilterOutputStream();

  wxStreamError LastError() const { return m_parent_o_stream->LastError(); }
  size_t StreamSize() const { return m_parent_o_stream->StreamSize(); }

 protected:
  wxOutputStream *m_parent_o_stream;
};

#endif
