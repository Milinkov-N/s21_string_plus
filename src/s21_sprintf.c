#include "s21_string.h"

void print_arg(char *str, int *char_count, char *arg) {
  while (*arg != '\0') {
    str[(*char_count)++] = *arg;
    arg++;
  }
  str[*char_count] = '\0';
}

void print_null(char *str, int *char_count) {
#if defined(__APPLE__) || defined(__MACH__)
  char *arg = "(null)";
#endif
#if defined(__linux__)
  char *arg = "(nil)";
#endif
  for (int i = 0; arg[i]; i++) {
    str[(*char_count)++] = arg[i];
  }
}

char *parse_str(const char *format, int *i, va_list *input, int char_count,
                int *free_flag, char formated_str[1024]) {
  char *arg = s21_NULL;
  st_f flag = {0};
  st_w width = {0};
  st_a accuracy = {0};
  st_l length = {0};
  check_elements(format, i, &flag, &width, &accuracy, &length, input);
  if (is_specifier(format[*i])) {
    st_s specifiers = {0};
    get_specifier(format[*i], &specifiers);
    if ((specifiers.u || specifiers.c || specifiers.s) && flag.space) {
      flag.space = 0;
    }
    if ((specifiers.u || specifiers.c || specifiers.s) && flag.plus) {
      flag.plus = 0;
    }
    arg = get_formated_str(&flag, &width, &accuracy, &length, &specifiers,
                           input, char_count, formated_str, free_flag);
#if defined(__linux__)
    if (specifiers.p && s21_strcmp(arg, "0x0") == 0) {
      arg = s21_NULL;
    }
#endif
  } else {
    arg = "";
    *free_flag = 0;
  }
  return arg;
}

void check_elements(const char *format, int *i, st_f *flag, st_w *width,
                    st_a *accuracy, st_l *length, va_list *input) {
  while (is_flag(format[(*i)])) {
    get_flag(format[(*i)++], flag);
  }
  if (is_width(format[(*i)])) {
    get_width(format, i, width, input);
  }
  if (format[*i] == '.') {
    get_accuracy(format, i, accuracy, input);
  }
  while (is_length(format[*i])) {
    get_length(format[(*i)++], length);
  }
}

char *get_formated_str(st_f *flag, st_w *width, st_a *accuracy, st_l *length,
                       st_s *specifiers, va_list *input, int char_count,
                       char formated_str[1024], int *free_flag) {
  int arg_len = 0;
  int cap = 1024;
  char *arg = formated_str;
  int free_calloc = 0;

  if (specifiers->c) {
    c_specif(input, arg, &arg_len);
  }
  if (specifiers->s) {
    arg = s_specif(input, arg, &arg_len, accuracy);
  }
  if (specifiers->d || specifiers->i || specifiers->u) {
    diu_specif(length, input, arg, &arg_len, specifiers, accuracy);
  }
  if (specifiers->o || specifiers->x || specifiers->X || specifiers->p) {
    oxX_specif(flag, length, input, arg, &arg_len, specifiers, accuracy);
  }
  if (specifiers->e || specifiers->E || specifiers->f || specifiers->g ||
      specifiers->G) {
    eEgG_specif(length, input, specifiers, arg, &arg_len, accuracy);
  }
  if (specifiers->n) {
    n_specifiers(input, char_count);
  }
  if (specifiers->percent) {
    arg[0] = '%';
  }
  if (flag->flag) {
    arg = make_flag(flag, arg, &arg_len, &cap, specifiers);
    free_calloc = 1;
    *free_flag = 1;
  }
  if (width->flag && accuracy->digit <= width->digit) {
    arg = make_width(width, flag, arg, arg_len, &cap, free_calloc);
    *free_flag = 1;
  }
  return arg;
}

char *make_flag(st_f *flag, char *arg, int *arg_len, int *cap,
                st_s *specifiers) {
  char *temp;
  *cap = *arg_len + 1 > *cap ? *cap + 10 : *cap;
  temp = calloc(*cap, sizeof(char));
  if (temp != s21_NULL) {
    if (flag->plus) {
      int negative = (arg[0] == '-') ? 1 : 0;
      int k = 0;
      for (int i = 0; i < *arg_len; i++) {
        if (!negative && !k) {
          temp[0] = '+';
          k = 1;
          *arg_len += 1;
        }
        temp[i + k] = arg[i];
      }
    } else if (flag->space) {
      if (arg[0] == '-') {
        s21_strcpy(temp, arg);
      } else {
        temp[0] = ' ';
        *arg_len += 1;
        s21_strcpy((temp + 1), arg);
      }
    } else if (flag->hash) {
      if (arg[0] == '0' && s21_strlen(arg) == 1) {
        temp[0] = '0';
      } else {
        if (specifiers->o) {
          temp[0] = '0';
          *arg_len += 1;
          s21_strcpy((temp + 1), arg);
        } else if (specifiers->x || specifiers->X) {
          if (specifiers->x) {
            temp[0] = '0';
            temp[1] = 'x';
            *arg_len += 2;
          } else if (specifiers->X) {
            temp[0] = '0';
            temp[1] = 'X';
            *arg_len += 2;
          }
          s21_strcpy((temp + 2), arg);
        }
        if ((specifiers->f || specifiers->g) && !s21_strchr(arg, '.')) {
          arg[*arg_len] = '.';
          *arg_len += 1;
          arg[*arg_len] = '\0';
          s21_strcpy(temp, arg);
        } else if ((specifiers->f || specifiers->g || specifiers->e ||
                    specifiers->E || specifiers->p)) {
          s21_strcpy(temp, arg);
        }
      }
    } else {
      s21_strcpy(temp, arg);
    }
  }
  return temp;
}

char *make_width(st_w *width, st_f *flag, char *arg, int arg_len, int *cap,
                 int free_calloc) {
  char *temp;
  *cap = width->digit > *cap ? width->digit + 10 : *cap;
  temp = calloc(*cap, sizeof(char));
  if (temp != s21_NULL) {
    if (flag->minus && width->digit > arg_len) {
      for (int j = 0; j < width->digit; j++) {
        if (j < arg_len) {
          temp[j] = arg[j];
        } else {
          temp[j] = ' ';
        }
      }
    } else if (width->digit > arg_len) {
      int sign = (arg[0] == '-' || arg[0] == '+' || arg[0] == ' ') ? 1 : 0;
      int sixteen = (arg[0] == '0' && (arg[1] == 'x' || arg[1] == 'X')) ? 1 : 0;
      for (int i = 0, j = 0; i < width->digit; i++) {
        if (sign && flag->zero) {
          temp[0] = arg[0];
          j++;
          i++;
          sign = 0;
          width->digit += 1;
        }
        if (sixteen && flag->zero) {
          temp[0] = '0';
          temp[1] = arg[1];
          sixteen = 0;
          i += 2;
          j += 2;
          width->digit += 2;
        }
        if (i < width->digit - arg_len) {
          temp[i] = flag->zero ? '0' : ' ';
        } else {
          temp[i] = arg[j++];
        }
      }
    } else if (width->digit <= arg_len) {
      s21_strcpy(temp, arg);
    }
  }
  if (free_calloc) {
    free(arg);
  }
  return temp;
}

char *s_specif(va_list *input, char *arg, int *arg_len, st_a *accuracy) {
  char *str = va_arg(*input, char *);
  if (str != s21_NULL) {
    int size_str = (int)s21_strlen(str);
    if (accuracy->flag && accuracy->digit < size_str) {
      size_str = accuracy->digit;
    }
    for (int i = 0; i < size_str; i++) {
      arg[*arg_len] = str[*arg_len];
      *arg_len += 1;
    }
  } else {
    arg[0] = '1';
  }
  return arg;
}

void c_specif(va_list *input, char *arg, int *arg_len) {
  char ch = va_arg(*input, int);
  arg[(*arg_len)++] = ch;
  arg[(*arg_len)] = '\0';
}

void diu_specif(st_l *length, va_list *input, char *arg, int *arg_len,
                st_s *specifiers, st_a *accuracy) {
  long long d = 1;
  unsigned long long dd = 1;
  int flag_unsign = 0;
  if (length->l == 2 && specifiers->u) {
    dd = (unsigned long long)va_arg(*input, long long);
    flag_unsign = 1;
  } else {
    d = get_d(length, specifiers, input);
  }
  char buf[64] = "";
  if (flag_unsign) {
    s21_ulltoa(dd, buf);
  } else {
    s21_lltoa(d, buf);
  }
  int i = 0;
  int temp_arg_len = 0;
  if (accuracy->flag) {
    int d_size = (int)s21_strlen(buf);
    for (i = 0; i < accuracy->digit - d_size; i++) {
      temp_arg_len += 1;
      arg[i] = '0';
    }
  }
  print_arg(arg + i, arg_len, buf);
  *arg_len += temp_arg_len;
  if (accuracy->flag && accuracy->digit == 0 && (dd == 0 || d == 0)) {
    arg[0] = '\0';
  }
}

void oxX_specif(st_f *flag, st_l *length, va_list *input, char *arg,
                int *arg_len, st_s *specifiers, st_a *accuracy) {
  unsigned long long dd;
  if (length->h) {
    dd = (unsigned short)va_arg(*input, int);
  } else if (length->l) {
    dd = va_arg(*input, unsigned long);
  } else if (specifiers->p) {
    dd = va_arg(*input, unsigned long long);
  } else {
    unsigned int temp = va_arg(*input, unsigned int);
    dd = temp;
  }
  char buf[64] = "";
  if (specifiers->o) {
    o_specif(&dd);
    s21_ulltoa(dd, buf);
  } else if (specifiers->x) {
    x_specif(dd, buf, 1);
  } else if (specifiers->X) {
    x_specif(dd, buf, 0);
  } else if (specifiers->p) {
    x_specif(dd, buf, 2);
  }
  int i = 0;
  int temp_arg_len = 0;
  if (dd == 0) {
    flag->hash = 0;
  }
  if (accuracy->flag) {
    int d_size = (int)s21_strlen(buf);
    temp_arg_len = *arg_len;
    for (i = 0; i < accuracy->digit - d_size; i++) {
      arg[i] = '0';
      temp_arg_len++;
    }
  }
  print_arg(arg + i, arg_len, buf);
  *arg_len += temp_arg_len;
  if (accuracy->flag && accuracy->digit == 0) {
    arg[0] = '\0';
  }
}

long long get_d(st_l *length, st_s *specifiers, va_list *input) {
  long long d;
  if (length->h == 1 && (specifiers->d || specifiers->i)) {
    d = (short)va_arg(*input, int);
  } else if (length->h == 1 && specifiers->u) {
    d = (unsigned short)va_arg(*input, int);
  } else if (length->h == 2 && specifiers->i) {
    char ch;
    ch = (char)va_arg(*input, int);
    d = (long long)ch;
  } else if (length->h == 2 && specifiers->u) {
    unsigned char ch;
    ch = (unsigned char)va_arg(*input, int);
    d = (long long)ch;
  } else if (!length->h && !length->l && !length->L && specifiers->u) {
    d = (unsigned int)va_arg(*input, int);
  } else if (length->l == 2 && (specifiers->i || specifiers->d)) {
    d = va_arg(*input, long long);
  } else if (length->l == 1) {
    d = va_arg(*input, long);
  } else {
    d = va_arg(*input, int);
  }
  return d;
}

void o_specif(unsigned long long int *dd) {
  unsigned long long int d_temp = *dd;
  unsigned long long int o_temp = 0;
  int i = 0;
  while (d_temp != 0) {
    int last_digit = (int)(d_temp % 8);
    o_temp += last_digit * (unsigned long long int)pow(10, i++);
    d_temp /= 8;
  }
  *dd = o_temp;
}

void x_specif(unsigned long long int dd, char *buf, int flag_x) {
  char temp[64] = "";
  int i = 0;
  int j = 0;
  if (dd == 0) {
    temp[64 - i - 1] = '0';
    i++;
  }
  while (dd != 0) {
    int last_digit = (int)(dd % 16);
    if (last_digit >= 10) {
      last_digit = flag_x ? last_digit + 87 : last_digit + 55;
      temp[64 - i - 1] = (char)(last_digit);
    } else {
      temp[64 - i - 1] = (char)(last_digit + 48);
    }
    i++;
    dd /= 16;
  }
  if (flag_x == 2) {
    temp[64 - i - 1] = 'x';
    i++;
    temp[64 - i - 1] = '0';
    i++;
  }
  for (; i > 0; j++, i--) {
    buf[j] = temp[64 - i];
  }
}

void eEgG_specif(st_l *length, va_list *input, st_s *specifiers, char *arg,
                 int *arg_len, st_a *accuracy) {
  long double ld;
  char buf[124] = "";
  if (length->L) {
    ld = va_arg(*input, long double);
  } else {
    ld = va_arg(*input, double);
  }
  accuracy->digit = accuracy->digit > 17 ? 17 : accuracy->digit;
  if (!accuracy->flag) accuracy->digit = 6;
  if (accuracy->flag && accuracy->digit == 0) {
    accuracy->digit = 0;
  }
  if (specifiers->g || specifiers->G) {
    gtoa(ld, buf, accuracy, specifiers);
  } else {
    ftoa(ld, buf, specifiers->e, specifiers->E, accuracy->digit);
  }
  print_arg(arg, arg_len, buf);
}

void n_specifiers(va_list *input, int char_count) {
  int *temp = va_arg(*input, int *);
  *temp = char_count;
}

int is_flag(char format) {
  char *flags = "-+ #0";
  return s21_strchr(flags, format) != s21_NULL;
}

int is_width(char format) {
  char *flags = "0123456789*";
  return s21_strchr(flags, format) != s21_NULL;
}

int is_length(char format) {
  char *flags = "hlL*";
  return s21_strchr(flags, format) != s21_NULL;
}

int is_specifier(char ch) {
  char *specifiers = "cdeEfgGosuxXpni%";
  return s21_strchr(specifiers, ch) != s21_NULL;
}

void get_flag(char format, st_f *flag) {
  if (format == '-' && flag->zero == 0) {
    flag->minus = 1;
  } else if (format == '0' && flag->minus == 0) {
    flag->zero = 1;
  } else if (format == '+' && flag->space == 0) {
    flag->plus = 1;
  } else if (format == ' ' && flag->plus == 0) {
    flag->space = 1;
  } else if (format == '#') {
    flag->hash = 1;
  }
  flag->flag = 1;
}

void get_width(const char *format, int *i, st_w *width, va_list *input) {
  if (format[*i] == '*') {
    width->digit = va_arg(*input, int);
    *i += 1;
  } else {
    width->digit = parse_digit(format, i);
  }
  width->flag = 1;
}

void get_accuracy(const char *format, int *i, st_a *accuracy, va_list *input) {
  *i += 1;
  if (format[*i] == '*') {
    accuracy->digit = va_arg(*input, int);
    accuracy->digit = accuracy->digit == 0 ? -1 : accuracy->digit;
    *i += 1;
  } else {
    if (!is_digit(format[*i])) {
      accuracy->digit = -1;
    } else {
      accuracy->digit = parse_digit(format, i);
    }
  }
  accuracy->flag = 1;
}

void get_length(char format, st_l *length) {
  if (format == 'h') {
    length->h += 1;
  } else if (format == 'l') {
    length->l += 1;
  } else if (format == 'L') {
    length->L += 1;
  }
  length->flag = 1;
}

void get_specifier(char format, st_s *specifiers) {
  if (format == 'c') {
    specifiers->c = 1;
  } else if (format == 'd') {
    specifiers->d = 1;
  } else if (format == 'i') {
    specifiers->i = 1;
  } else if (format == 'e') {
    specifiers->e = 1;
  } else if (format == 'E') {
    specifiers->E = 1;
  } else if (format == 'f') {
    specifiers->f = 1;
  } else if (format == 'g') {
    specifiers->g = 1;
  } else if (format == 'G') {
    specifiers->G = 1;
  } else if (format == 'o') {
    specifiers->o = 1;
  } else if (format == 's') {
    specifiers->s = 1;
  } else if (format == 'u') {
    specifiers->u = 1;
  } else if (format == 'x') {
    specifiers->x = 1;
  } else if (format == 'X') {
    specifiers->X = 1;
  } else if (format == 'p') {
    specifiers->p = 1;
  } else if (format == 'n') {
    specifiers->n = 1;
  } else if (format == '%') {
    specifiers->percent = 1;
  }
}

int parse_digit(const char *format, int *i) {
  int number = 0;
  for (int j = 0; j < 10; j++) {
    number += digit_to_int(format[(*i)++]);
    if (!is_digit(format[*i])) {
      break;
    }
    number *= 10;
  }
  return number;
}

int digit_to_int(char d) {
  int digit;
  switch (d) {
    case '1':
      digit = 1;
      break;
    case '2':
      digit = 2;
      break;
    case '3':
      digit = 3;
      break;
    case '4':
      digit = 4;
      break;
    case '5':
      digit = 5;
      break;
    case '6':
      digit = 6;
      break;
    case '7':
      digit = 7;
      break;
    case '8':
      digit = 8;
      break;
    case '9':
      digit = 9;
      break;
    default:
      digit = 0;
  }
  return digit;
}

int is_digit(char format) {
  char *digits = "0123456789";
  int temp = 0;
  if (format <= 0)
    temp = 0;
  else
    temp = s21_strchr(digits, format) != s21_NULL;
  return temp;
}

void ftoa(long double ld, char *res, int flag_e, int flag_E, int accuracy) {
  int e = 0;
  if (flag_e || flag_E) {
    e = remake_double(&ld);
  }
  long long ipart = (long long)ld;
  long double fpart = (long double)(ld - ipart);
  if (accuracy <= 0) {
    ipart += fpart >= 0.5 ? 1 : 0;
  }
  s21_lltoa(ipart, res);
  int temp = 0;
  for (int i = 0; res[i]; i++) {
    if (res[i + 1] == '\0') {
      temp = i + 1;
    }
  }
  if (accuracy > 0) {
    res[temp++] = '.';
    fpart = round(fpart * pow(10, accuracy));
    fpart *= fpart < 0 ? -1 : 1;
    char buf[128] = "";
    s21_lltoa((long long)fpart, buf);
    int buf_len = (int)s21_strlen(buf);
    for (int i = 0; i < accuracy - buf_len; i++) {
      res[temp++] = '0';
    }
    for (int i = 0; buf[i]; i++) {
      res[temp++] = buf[i];
    }
  }
  if (flag_e || flag_E) {
    add_eE(flag_e, flag_E, res, &temp, e);
  }
  res[temp] = '\0';
}

void gtoa(long double ld, char *res, st_a *accuracy, st_s *specifiers) {
  long long temp_ipart = (long long)ld;
  int more_six = 0, six = 0, digits_ipart = 0, digits_fpart = 0;

  while (temp_ipart != 0) {
    temp_ipart /= 10;
    digits_ipart++;
  }

  long long ipart = (long long)ld;
  double fpart = (double)(ld - ipart);

  double temp_fpart = fpart;
  while ((int)temp_fpart == 0 && digits_fpart < 30) {
    temp_fpart *= 10;
    digits_fpart++;
  }

  if ((digits_ipart > accuracy->digit && digits_ipart != 0) ||
      (digits_fpart >= 6 && ipart == 0)) {
    more_six = remake_double(&ld);
    ipart = (long long)ld;
    fpart = (double)(ld - ipart);
    if (fpart >= 0.5) {
      ipart += 1;
    }
  }

  if (digits_ipart == accuracy->digit && digits_ipart != 0) {
    six = 1;
    if (fpart >= 0.5) {
      ipart += 1;
    }
  }
  s21_lltoa(ipart, res);
  int temp = (int)s21_strlen(res);
  if ((accuracy->digit > 0 ||
       (accuracy->digit <= 0 && digits_ipart == 0 && accuracy->flag)) &&
      !more_six) {
    gtoa_accuracy(six, accuracy->digit, res, &temp, more_six, &fpart,
                  digits_ipart, digits_fpart);
  }
  if (more_six) {
    if (specifiers->g)
      add_eE(1, 0, res, &temp, more_six);
    else
      add_eE(0, 1, res, &temp, more_six);
  }
  res[temp] = '\0';
}

void gtoa_accuracy(int six, int accuracy, char *res, int *temp, int more_six,
                   double *fpart, int digits_ipart, int digits_fpart) {
  if ((!six) || accuracy <= 0) {
    res[(*temp)++] = '.';
    int j = digits_fpart;
    while (j != 1) {
      res[(*temp)++] = '0';
      j--;
    }
    if (more_six) {
      *fpart = round(*fpart * pow(10, accuracy - 1));
    } else if (accuracy <= 0) {
      *fpart = round(*fpart * pow(10, digits_fpart));
    } else {
      *fpart = round(*fpart * pow(10, accuracy - digits_ipart));
    }
    char buf[128] = "";
    s21_lltoa((long long)*fpart, buf);
    for (int i = 0; buf[i]; i++) {
      res[(*temp)++] = buf[i];
    }
    del_zero(res, *temp);
  }
}

void del_zero(char *res, int last) {
  if (res[last - 1] == '0') {
    res[last - 1] = '\0';
    del_zero(res, last - 1);
  } else if (res[last - 1] == '.') {
    res[last - 1] = '\0';
  }
}

void add_eE(int flag_e, int flag_E, char *res, int *temp, int e) {
  if (flag_e) {
    res[(*temp)++] = 'e';
  }
  if (flag_E) {
    res[(*temp)++] = 'E';
  }
  res[(*temp)++] = e >= 0 ? '+' : '-';
  e *= e < 0 ? -1 : 1;
  char buf_e[64] = "";
  s21_lltoa(e, buf_e);
  if (e < 10) {
    res[(*temp)++] = '0';
  }
  for (int i = 0; buf_e[i]; i++) {
    res[(*temp)++] = buf_e[i];
  }
}

int remake_double(long double *f) {
  int e = 0;
  int temp = 0;
  if ((int)*f == 0) {
    while ((int)*f == 0) {
      *f *= 10;
      e++;
      temp++;
      if (temp > 20) {
        e = 0;
        break;
      }
    }
    e *= -1;
  } else {
    while ((int)*f != 0) {
      *f = *f / 10;
      e++;
    }
    *f *= 10;
    e--;
  }
  return e;
}

void s21_lltoa(long long i, char buf[64]) {
  char temp_buf[64] = "";
  int j = 0;
  int negative = 0;
  if (i < 0) {
    negative = 1;
    i = -(i);
  }
  do {
    long long temp_i = i % 10;
    i /= 10;
    temp_buf[j++] = (char)(temp_i + 48);
  } while (i != 0);
  if (negative) temp_buf[j++] = '-';
  int k = 0;
  for (k = 0; k < j; k++) {
    buf[k] = temp_buf[j - k - 1];
  }
}

void s21_ulltoa(unsigned long long i, char buf[64]) {
  char temp_buf[64] = "";
  int j = 0;
  do {
    unsigned long long temp_i = i % 10;
    i /= 10;
    temp_buf[j++] = (char)(temp_i + 48);
  } while (i != 0);
  for (int k = 0; k < j; k++) {
    buf[k] = temp_buf[j - k - 1];
  }
}
