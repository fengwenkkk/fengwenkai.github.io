#ifndef ZipUtilH
#define ZipUtilH

BOOL ZipDir(CHAR *szRootDir, VECTOR<std::string> &vSubDir, VECTOR<std::string> &vFilePostfix, CHAR *szZipFile);
BOOL UnzipDir(CHAR *szRootDir, CHAR *szZipFile);

BOOL ZipFile(CHAR *szFile, CHAR *szZipFile);
BOOL UnzipFile(CHAR *szZipFile, CHAR *szFile);

//---------------------------------------------------------------------------
#endif
