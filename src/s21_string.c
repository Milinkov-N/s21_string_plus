#include "s21_string.h"

int s21_sprintf(char *str, const char *format, ...) {
  int char_count = 0;
  va_list(input);
  va_start(input, format);
  int n = (int)s21_strlen(format);
  for (int i = 0; format[i] != '\0' && i < n; i++) {
    char formated_str[1024] = {'\0'};
    int free_flag = 0;
    if (format[i] == '%') {
      i += 1;
      char *arg =
          parse_str(format, &i, &input, char_count, &free_flag, formated_str);
      if (arg != s21_NULL) {
        print_arg(str, &char_count, arg);
        if (free_flag) free(arg);
      } else {
        print_null(str, &char_count);
      }
    } else {
      str[char_count] = format[i];
      char_count++;
    }
  }
  va_end(input);
  str[char_count] = '\0';
  return char_count;
}

void *s21_memchr(const void *str, int c, s21_size_t n) {
  void *res = s21_NULL;
  unsigned char *p = (unsigned char *)str;
  int flag = 1;
  for (s21_size_t i = 0; i < n && flag; i++) {
    if (*p == (unsigned char)c) {
      flag = 0;
      res = p;
    } else
      p++;
  }
  return res;
}

int s21_memcmp(const void *str1, const void *str2, s21_size_t n) {
  int isFindedDiff = 0;
  int res = 0;
  for (s21_size_t i = 0; i < n && !isFindedDiff; i++) {
    if (*(unsigned char *)(str1 + i) != *(unsigned char *)(str2 + i)) {
      isFindedDiff = 1;
      res = *(unsigned char *)(str1 + i) - *(unsigned char *)(str2 + i);
    }
  }
  return res;
}

void *s21_memcpy(void *dest, const void *src, s21_size_t n) {
  s21_size_t i;
  for (i = 0; i < n; i++)
    ((unsigned char *)dest)[i] = ((unsigned char *)src)[i];
  return dest;
}

void *s21_memmove(void *dest, const void *src, s21_size_t n) {
  void *res = dest;
  if (dest <= src || (unsigned char *)dest >= ((unsigned char *)src + n)) {
    for (s21_size_t i = 0; i < n; i++) {
      *(unsigned char *)dest = *(unsigned char *)src;
      dest = (unsigned char *)dest + 1;
      src = (unsigned char *)src + 1;
    }
  } else {
    dest = (unsigned char *)dest + n - 1;
    src = (unsigned char *)src + n - 1;
    for (s21_size_t i = 0; i < n; i++) {
      *(unsigned char *)dest = *(unsigned char *)src;
      dest = (unsigned char *)dest - 1;
      src = (unsigned char *)src - 1;
    }
  }
  return res;
}

void *s21_memset(void *str, int c, s21_size_t n) {
  for (s21_size_t i = 0; i < n; i++) {
    *(char *)(str + i) = c;
  }
  return str;
}

char *s21_strcat(char *dest, const char *src) {
  int len_dest = (int)s21_strlen(dest);
  int len_src = (int)s21_strlen(src);
  int i;
  for (i = len_dest; i - len_dest < len_src; i++) {
    dest[i] = src[i - len_dest];
  }
  dest[i] = '\0';
  return dest;
}

char *s21_strchr(const char *str, int c) {
  if (str != s21_NULL) {
    while (*str) {
      if (*str == c) {
        break;
      }
      str++;
    }
  }
  if (*str == '\0') str = s21_NULL;
  return (char *)str;
}

int s21_strcmp(const char *str1, const char *str2) {
  int res = 0;
  int flag = 1;
  s21_size_t i;
  s21_size_t len_str1 = s21_strlen(str1);
  s21_size_t len_str2 = s21_strlen(str2);
  for (i = 0; str1[i] && str2[i]; i++) {
    if (str1[i] != str2[i]) {
      res = str1[i] - str2[i];
      flag = 0;
      break;
    }
  }
  if ((len_str1 > len_str2 || len_str2 > len_str1) && flag) {
    res = len_str1 > len_str2 ? str1[len_str2] : -str2[len_str1];
  }

  return res;
}

char *s21_strcpy(char *dest, const char *src) {
  if (src != s21_NULL && dest != s21_NULL) {
    int i;
    for (i = 0; src[i] || i == 0; i++) {
      dest[i] = src[i];
    }
    if (s21_strlen(dest) > s21_strlen(src)) {
      dest[i] = '\0';
    }
  }
  return dest;
}

char *s21_strpbrk(const char *str1, const char *str2) {
  int flag = 1;
  s21_size_t len_str1 = s21_strlen(str1);
  s21_size_t len_str2 = s21_strlen(str2);
  char *res = s21_NULL;
  for (s21_size_t i = 0; i < len_str1 && flag; i++) {
    for (s21_size_t j = 0; j < len_str2 && flag; j++) {
      if (str1[i] == str2[j]) {
        res = (char *)(str1 + i);
        flag = 0;
      }
    }
  }
  return res;
}

char *s21_strerror(int errnum) {
#if defined(__APPLE__) || defined(__MACH__)
  static const int errorsLen = 106;
  int isLinux = 0;
  static const char errors[][100] = {
      "Undefined error: 0",
      "Operation not permitted",
      "No such file or directory",
      "No such process",
      "Interrupted system call",
      "Input/output error",
      "Device not configured",
      "Argument list too long",
      "Exec format error",
      "Bad file descriptor",
      "No child processes",
      "Resource deadlock avoided",
      "Cannot allocate memory",
      "Permission denied",
      "Bad address",
      "Block device required",
      "Resource busy",
      "File exists",
      "Cross-device link",
      "Operation not supported by device",
      "Not a directory",
      "Is a directory",
      "Invalid argument",
      "Too many open files in system",
      "Too many open files",
      "Inappropriate ioctl for device",
      "Text file busy",
      "File too large",
      "No space left on device",
      "Illegal seek",
      "Read-only file system",
      "Too many links",
      "Broken pipe",
      "Numerical argument out of domain",
      "Result too large",
      "Resource temporarily unavailable",
      "Operation now in progress",
      "Operation already in progress",
      "Socket operation on non-socket",
      "Destination address required",
      "Message too long",
      "Protocol wrong type for socket",
      "Protocol not available",
      "Protocol not supported",
      "Socket type not supported",
      "Operation not supported",
      "Protocol family not supported",
      "Address family not supported by protocol family",
      "Address already in use",
      "Can't assign requested address",
      "Network is down",
      "Network is unreachable",
      "Network dropped connection on reset",
      "Software caused connection abort",
      "Connection reset by peer",
      "No buffer space available",
      "Socket is already connected",
      "Socket is not connected",
      "Can't send after socket shutdown",
      "Too many references: can't splice",
      "Operation timed out",
      "Connection refused",
      "Too many levels of symbolic links",
      "File name too long",
      "Host is down",
      "No route to host",
      "Directory not empty",
      "Too many processes",
      "Too many users",
      "Disc quota exceeded",
      "Stale NFS file handle",
      "Too many levels of remote in path",
      "RPC struct is bad",
      "RPC version wrong",
      "RPC prog. not avail",
      "Program version wrong",
      "Bad procedure for program",
      "No locks available",
      "Function not implemented",
      "Inappropriate file type or format",
      "Authentication error",
      "Need authenticator",
      "Device power is off",
      "Device error",
      "Value too large to be stored in data type",
      "Bad executable (or shared library)",
      "Bad CPU type in executable",
      "Shared library version mismatch",
      "Malformed Mach-o file",
      "Operation canceled",
      "Identifier removed",
      "No message of desired type",
      "Illegal byte sequence",
      "Attribute not found",
      "Bad message",
      "EMULTIHOP (Reserved)",
      "No message available on STREAM",
      "ENOLINK (Reserved)",
      "No STREAM resources",
      "Not a STREAM",
      "Protocol error",
      "STREAM ioctl timeout",
      "Operation not supported on socket",
      "Policy not found",
      "State not recoverable",
      "Previous owner died",
      "Interface output queue is full"};
#endif

#if defined(__linux__)
  static const int errorsLen = 133;
  int isLinux = 1;
  static const char errors[][100] = {
      "Success",
      "Operation not permitted",
      "No such file or directory",
      "No such process",
      "Interrupted system call",
      "Input/output error",
      "No such device or address",
      "Argument list too long",
      "Exec format error",
      "Bad file descriptor",
      "No child processes",
      "Resource temporarily unavailable",
      "Cannot allocate memory",
      "Permission denied",
      "Bad address",
      "Block device required",
      "Device or resource busy",
      "File exists",
      "Invalid cross-device link",
      "No such device",
      "Not a directory",
      "Is a directory",
      "Invalid argument",
      "Too many open files in system",
      "Too many open files",
      "Inappropriate ioctl for device",
      "Text file busy",
      "File too large",
      "No space left on device",
      "Illegal seek",
      "Read-only file system",
      "Too many links",
      "Broken pipe",
      "Numerical argument out of domain",
      "Numerical result out of range",
      "Resource deadlock avoided",
      "File name too long",
      "No locks available",
      "Function not implemented",
      "Directory not empty",
      "Too many levels of symbolic links",
      "Unknown error 41",
      "No message of desired type",
      "Identifier removed",
      "Channel number out of range",
      "Level 2 not synchronized",
      "Level 3 halted",
      "Level 3 reset",
      "Link number out of range",
      "Protocol driver not attached",
      "No CSI structure available",
      "Level 2 halted",
      "Invalid exchange",
      "Invalid request descriptor",
      "Exchange full",
      "No anode",
      "Invalid request code",
      "Invalid slot",
      "Unknown error 58",
      "Bad font file format",
      "Device not a stream",
      "No data available",
      "Timer expired",
      "Out of streams resources",
      "Machine is not on the network",
      "Package not installed",
      "Object is remote",
      "Link has been severed",
      "Advertise error",
      "Srmount error",
      "Communication error on send",
      "Protocol error",
      "Multihop attempted",
      "RFS specific error",
      "Bad message",
      "Value too large for defined data type",
      "Name not unique on network",
      "File descriptor in bad state",
      "Remote address changed",
      "Can not access a needed shared library",
      "Accessing a corrupted shared library",
      ".lib section in a.out corrupted",
      "Attempting to link in too many shared libraries",
      "Cannot exec a shared library directly",
      "Invalid or incomplete multibyte or wide character",
      "Interrupted system call should be restarted",
      "Streams pipe error",
      "Too many users",
      "Socket operation on non-socket",
      "Destination address required",
      "Message too long",
      "Protocol wrong type for socket",
      "Protocol not available",
      "Protocol not supported",
      "Socket type not supported",
      "Operation not supported",
      "Protocol family not supported",
      "Address family not supported by protocol",
      "Address already in use",
      "Cannot assign requested address",
      "Network is down",
      "Network is unreachable",
      "Network dropped connection on reset",
      "Software caused connection abort",
      "Connection reset by peer",
      "No buffer space available",
      "Transport endpoint is already connected",
      "Transport endpoint is not connected",
      "Cannot send after transport endpoint shutdown",
      "Too many references: cannot splice",
      "Connection timed out",
      "Connection refused",
      "Host is down",
      "No route to host",
      "Operation already in progress",
      "Operation now in progress",
      "Stale file handle",
      "Structure needs cleaning",
      "Not a XENIX named type file",
      "No XENIX semaphores available",
      "Is a named type file",
      "Remote I/O error",
      "Disk quota exceeded",
      "No medium found",
      "Wrong medium type",
      "Operation canceled",
      "Required key not available",
      "Key has expired",
      "Key has been revoked",
      "Key was rejected by service",
      "Owner died",
      "State not recoverable",
      "Operation not possible due to RF-kill",
      "Memory page has hardware error"};
#endif
  static char res[100] = {'\0'};
  if (errnum > errorsLen || errnum < 0) {
    if (isLinux) {
      sprintf(res, "Unknown error %i", errnum);
    } else {
      sprintf(res, "Unknown error: %i", errnum);
    }

  } else {
    s21_strcpy(res, (char *)errors[errnum]);
  }
  return res;
}

s21_size_t s21_strlen(const char *str) {
  s21_size_t len = 0;
  if (str != s21_NULL) {
    for (; str[len]; len++)
      ;
  }
  return len;
}

char *s21_strncat(char *dest, const char *src, s21_size_t n) {
  int i;
  int len_dest = (int)s21_strlen(dest);
  for (i = 0; src[i] != '\0' && i < (int)n; i++) {
    dest[len_dest + i] = src[i];
  }
  dest[len_dest + i] = '\0';
  return dest;
}

int s21_strncmp(const char *str1, const char *str2, s21_size_t n) {
  int res = 0;
  if (n != 0) {
    if (str2[0] == '\0' || str1[0] == '\0') {
      res = str2[0] == '\0' ? (int)str1[0] : -(int)str2[0];
    } else {
      for (s21_size_t i = 0; i < n && str1[i] && str2[i]; i++) {
        if (str1[i] != str2[i]) {
          res = str1[i] - str2[i];
          break;
        }
        if (str1[i + 1] != '\0' && str2[i + 1] == '\0') {
          res = (int)str1[i + 1];
        } else if (str1[i + 1] == '\0' && str2[i + 1] != '\0') {
          res = -(int)str2[i + 1];
        }
      }
    }
  }
  return res;
}

char *s21_strncpy(char *dest, const char *src, s21_size_t n) {
  s21_size_t i;
  for (i = 0; src[i] != '\0' && i < n; i++) {
    dest[i] = src[i];
  }
  if (src[i] == '\0' && n > 0 && n > s21_strlen(src)) {
    dest[i] = '\0';
  }
  return dest;
}

char *s21_strrchr(const char *str, int c) {
  char *res = s21_NULL;
  for (s21_size_t i = 0; i < s21_strlen(str) + 1; i++) {
    if (str[i] == c) {
      res = (char *)(str + i);
    }
  }
  return res;
}

s21_size_t s21_strspn(const char *str1, const char *str2) {
  s21_size_t res = 0;
  s21_size_t len_str1 = s21_strlen(str1);
  s21_size_t len_str2 = s21_strlen(str2);
  for (s21_size_t i = 0; i < len_str1; i++) {
    int flag = 0;
    for (s21_size_t j = 0; j < len_str2 && !flag; j++) {
      if (str1[i] == str2[j]) {
        flag = 1;
        res++;
      }
    }
    if (!flag) {
      break;
    }
  }
  return res;
}

s21_size_t s21_strcspn(const char *str1, const char *str2) {
  int flag = 1;
  s21_size_t len_str1 = s21_strlen(str1);
  s21_size_t len_str2 = s21_strlen(str2);
  s21_size_t res = len_str1;
  for (s21_size_t i = 0; i < len_str1 && flag; i++) {
    for (s21_size_t j = 0; j < len_str2 && flag; j++) {
      if (str1[i] == str2[j]) {
        res = i;
        flag = 0;
      }
    }
  }
  return res;
}

char *s21_strstr(const char *haystack, const char *needle) {
  char *res = s21_NULL;
  if (needle[0] == '\0') {
    res = (char *)haystack;
  } else {
    int found = 0;
    for (int i = 0; haystack[i] && !found; i++) {
      int flag = 1;
      if (haystack[i] == needle[0]) {
        for (int j = 0; needle[j] && flag; j++) {
          if (haystack[i + j] != needle[j]) {
            flag = 0;
          } else if (needle[j + 1] == '\0') {
            res = (char *)(haystack + i);
            found = 1;
          }
        }
      }
    }
  }
  return res;
}

char *s21_strtok(char *str, const char *delim) {
  static char *token = 0;
  char *res = s21_NULL;
  if (str != s21_NULL) {
    token = str;
  } else if (token == NULL) {
    res = NULL;
  }
  if (token != NULL) {
    str = token + s21_strspn(token, delim);
    token = str + s21_strcspn(str, delim);
    res = str;
    if (*token) {
      *token = 0;
      token += 1;
    }
    if (token == str) {
      token = 0;
      res = 0;
    }
  }
  return res;
}

void *s21_to_upper(const char *str) {
  char *result = s21_NULL;
  if (str != s21_NULL) {
    int size = (int)s21_strlen(str);
    result = calloc(size + 1, sizeof(char));
    if (result != s21_NULL) {
      s21_strcpy(result, str);
      for (int i = 0; i < size; i++) {
        if (result[i] >= 97 && result[i] <= 122) {
          result[i] -= 32;
        }
      }
    }
  }
  return result;
}

void *s21_to_lower(const char *str) {
  char *result = s21_NULL;
  if (str != s21_NULL) {
    int size = (int)s21_strlen(str);
    result = calloc(size + 1, sizeof(char));
    if (result != s21_NULL) {
      s21_strcpy(result, str);
      for (int i = 0; i < size; i++) {
        if (result[i] >= 65 && result[i] <= 90) {
          result[i] += 32;
        }
      }
    }
  }
  return result;
}

void *s21_insert(const char *src, const char *str, s21_size_t start_index) {
  char *result = s21_NULL;
  if (src != s21_NULL) {
    s21_size_t size_str = s21_strlen(str);
    s21_size_t size_src = s21_strlen(src);
    s21_size_t size_result = size_src + size_str + 1;
    result = calloc(size_result, sizeof(char));
    if (str == NULL) {
      s21_strcpy(result, src);
    } else {
      if (result != s21_NULL && start_index < size_src) {
        for (s21_size_t i = 0, j = 0; j < size_result; j++, i++) {
          if (j == start_index) {
            s21_strcpy(result + j, str);
            j = size_str + j - 1;
            i--;
          } else {
            result[j] = src[i];
          }
        }
      } else {
        free(result);
        result = s21_NULL;
      }
    }
  }
  return result;
}

void *s21_trim(const char *src, const char *trim_chars) {
  char *result = s21_NULL;
  if (src != s21_NULL) {
    if (trim_chars == NULL || trim_chars[0] == '\0') {
      trim_chars = " ";
    }
    int size = (int)s21_strlen(src);
    if (size != 0) {
      int start = -1, end = -1;
      int flag_i = 1, flag_j = 1;
      for (int i = 0, j = size - 1; i < size; i++, j--) {
        if (s21_strchr(trim_chars, src[i]) == s21_NULL && flag_i) {
          start = i;
          flag_i = 0;
        }
        if (s21_strchr(trim_chars, src[j]) == s21_NULL && flag_j) {
          end = j;
          flag_j = 0;
        }
        if (!flag_i && !flag_j) {
          break;
        }
      }
      result = calloc(end - start + 2, sizeof(char));
      if (result != s21_NULL) {
        for (int i = start, j = 0; i <= end; i++, j++) {
          result[j] = src[i];
        }
      }
    } else {
      result = calloc(2, sizeof(char));
      result[0] = '\0';
    }
  }
  return result;
}