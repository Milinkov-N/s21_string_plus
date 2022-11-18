//
// Created by Maslyn Emperor on 11/8/22.
//
#include <math.h>

#include "s21_string.h"

int s21_sscanf(const char *str, const char *format, ...) {
  int scan_count = 0;
  int flag_while = 1;
  int error = 0;
  va_list(input);
  va_start(input, format);
  int format_ch_count = 0;
  int str_ch_count = 0;
  if (str == s21_NULL || format == s21_NULL || str[0] == '\0' ||
      is_empty_str(str)) {
    scan_count = -1;
  } else {
    scan_format(str, format, &str_ch_count, &format_ch_count, &flag_while,
                &error, &input, &scan_count);
    while (format[format_ch_count] != '\0') {
      if (format[format_ch_count] == '%' &&
          format[format_ch_count + 1] == 'n') {
        int *d = va_arg(input, int *);
        *d = str_ch_count;
        break;
      }
      format_ch_count++;
    }
  }
  return scan_count;
}

void scan_format(const char *str, const char *format, int *str_ch_count,
                 int *format_ch_count, int *flag_while, int *error,
                 va_list *input, int *scan_count) {
  while (*flag_while && !(*error) && str[*str_ch_count]) {
    if ((str[*str_ch_count] == ' ' || str[*str_ch_count] == '\t' ||
         str[*str_ch_count] == '\n') &&
        !(format[*format_ch_count] == '%' &&
          format[*format_ch_count + 1] == 'c')) {
      *str_ch_count += 1;
      continue;
    }
    if (format[*format_ch_count] == '%') {
      (*format_ch_count)++;
      if (format[*format_ch_count] != 'c') {
        skip_empty(str, str_ch_count);
      }
      int flag_star = format[*format_ch_count] == '*' ? 1 : 0;
      *error = scan_variable(str, format, format_ch_count, str_ch_count, input);
      if (!(*error) && format[*format_ch_count - 1] != 'n' &&
          format[*format_ch_count - 1] != '%' && !flag_star)
        (*scan_count)++;
    } else if ((format[*format_ch_count] == ' ' && str[*str_ch_count] != ' ') ||
               (format[*format_ch_count] == '\t' &&
                str[*str_ch_count] != '\t')) {
      (*format_ch_count)++;
    } else if (str[(*str_ch_count)++] != format[(*format_ch_count)++]) {
      *flag_while = 0;
    }
  }
}
void skip_empty(const char *str, int *str_ch_count) {
  while (1) {
    if (str[*str_ch_count] == '\t' || str[*str_ch_count] == '\r' ||
        str[*str_ch_count] == '\n') {
      *str_ch_count += 1;
    } else {
      break;
    }
  }
}

int is_empty_str(const char *str) {
  int result = 1;
  while (*str) {
    if (*str != ' ') {
      result = 0;
    }
    str += 1;
  }
  return result;
}

int scan_variable(const char *str, const char *format, int *format_ch_count,
                  int *str_ch_count, va_list *input) {
  int error = 0;
  st_f flag = {0};
  st_w width = {0};
  st_a accuracy = {0};
  st_l length = {0};
  int flag_star = 0;
  char formated_arg[512] = {'\0'};
  if (format[*format_ch_count] == '*') {
    *format_ch_count += 1;
    flag_star = 1;
  }
  check_elements(format, format_ch_count, &flag, &width, &accuracy, &length,
                 input);
  if (is_specifier(format[*format_ch_count])) {
    st_s specifiers = {0};
    get_specifier(format[*format_ch_count], &specifiers);
    *format_ch_count += 1;
    char *arg =
        formated_str(str, &specifiers, str_ch_count, &width, formated_arg);
    if (arg == s21_NULL) {
      error = 1;
    } else if (!flag_star) {
      set_variable(arg, &specifiers, &length, input);
    }
  } else {
    error = 1;
  }
  return error;
}

char *formated_str(const char *str, st_s *specifiers, int *str_ch_count,
                   st_w *width, char formated_arg[512]) {
  int error = 0;
  int cap = 512;
  int arg_ch_count = 0;
  char *arg = formated_arg;
  if (specifiers->c) {
    get_c(str, str_ch_count, arg, &arg_ch_count);
  } else if (specifiers->s) {
    get_s_str(str, str_ch_count, arg, &arg_ch_count, width);
  } else if (specifiers->d || specifiers->u) {
    error = get_d_str(str, str_ch_count, arg, &cap, &arg_ch_count, width);
  } else if (specifiers->f || specifiers->G || specifiers->g || specifiers->E ||
             specifiers->e) {
    error = get_f_str(str, str_ch_count, arg, &cap, &arg_ch_count, width);
  } else if (specifiers->o) {
    error = get_o_str(str, str_ch_count, arg, &cap, &arg_ch_count, width);
  } else if (specifiers->x || specifiers->X || specifiers->p) {
    error = get_x_str(str, str_ch_count, arg, &cap, &arg_ch_count, width);
  } else if (specifiers->i) {
    error = get_i_str(str, str_ch_count, arg, &cap, &arg_ch_count, width);
  } else if (specifiers->n) {
    get_n(*str_ch_count, arg, &arg_ch_count);
  } else if (specifiers->percent) {
    arg[0] = '%';
    *str_ch_count += 1;
  }
  if (error || (arg[0] == '-' && s21_strlen(arg) == 1 && !specifiers->c)) {
    arg = s21_NULL;
    arg_ch_count = 0;
  } else {
    arg[arg_ch_count] = '\0';
  }
  return arg;
}

void set_variable(char *arg, st_s *specifiers, st_l *length, va_list *input) {
  if (specifiers->c) {
    char *ch = va_arg(*input, char *);
    *ch = arg[0];
  } else if (specifiers->s) {
    set_s(arg, input);
  } else if (specifiers->d || specifiers->n || specifiers->u) {
    set_d(arg, specifiers, length, input);
  } else if (specifiers->f || specifiers->G || specifiers->g || specifiers->E ||
             specifiers->e) {
    set_f(arg, length, input);
  } else if (specifiers->o || specifiers->x || specifiers->X || specifiers->p) {
    set_oxXp(arg, length, input, specifiers);
  } else if (specifiers->i) {
    set_i(arg, length, input, specifiers);
  }
}

void get_n(int count, char *arg, int *arg_ch_count) {
  char buf[64] = "";
  s21_lltoa(count, buf);
  while (is_digit(buf[*arg_ch_count])) {
    arg[*arg_ch_count] = buf[*arg_ch_count];
    *arg_ch_count += 1;
  }
}

int get_numeric_system(const char *str, int *str_ch_count, char *arg,
                       int *arg_ch_count, st_w *width) {
  int numeric_system = 10;
  if (str[*str_ch_count] == '0') {
    arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
    if ((width->flag && *arg_ch_count < width->digit) || !width->flag) {
      if (str[*str_ch_count] == 'x' || str[*str_ch_count] == 'X') {
        numeric_system = 16;
        *str_ch_count += 1;
      } else {
        numeric_system = 8;
      }
    }
  }
  return numeric_system;
}

int get_i_str(const char *str, int *str_ch_count, char *arg, const int *cap,
              int *arg_ch_count, st_w *width) {
  int error = 0, while_flag = 1;

  if (str[*str_ch_count] == '-')
    arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
  else if (str[*str_ch_count] == '+')
    *str_ch_count += 1;

  int numeric_system =
      get_numeric_system(str, str_ch_count, arg, arg_ch_count, width);
  int i = numeric_system == 16 ? 1 + *arg_ch_count : *arg_ch_count;

  while (while_flag && !error) {
    if (width->flag && i >= width->digit) break;
    if (is_digit(str[*str_ch_count])) {
      if (numeric_system == 8) {
        int temp = digit_to_int(str[*str_ch_count]);
        if (temp == 8 || temp == 9) break;
      }
      arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
    } else if (numeric_system == 16 &&
               s21_strchr("AaBbCcDdEeFf", str[*str_ch_count])) {
      arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
    } else
      while_flag = 0;
    if (*arg_ch_count == *cap) error = 1;
    i += 1;
  }
  if (arg[0] == '\0') error = 1;
  if (numeric_system == 10)
    arg[(*arg_ch_count)++] = 'd';
  else
    arg[(*arg_ch_count)++] = numeric_system == 8 ? 'o' : 'x';
  return error;
}

void set_i(char *arg, st_l *length, va_list *input, st_s *specifiers) {
  int size = (int)s21_strlen(arg);
  if (arg[size - 1] == 'o') {
    specifiers->o = 1;
  } else if (arg[size - 1] == 'x') {
    specifiers->x = 1;
  }
  if (arg[size - 1] == 'd') {
    arg[size - 1] = '\0';
    set_d(arg, specifiers, length, input);
  } else {
    arg[size - 1] = '\0';
    set_oxXp(arg, length, input, specifiers);
  }
}

void get_s_str(const char *str, int *str_ch_count, char *arg, int *arg_ch_count,
               st_w *width) {
  while (str[*str_ch_count] != ' ' && str[*str_ch_count] != '\n' &&
         str[*str_ch_count] != '\0' && str[*str_ch_count] != '\t') {
    arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
    if (width->flag && *arg_ch_count == width->digit) {
      break;
    }
  }
}

void set_s(const char *arg, va_list *input) {
  char *str = va_arg(*input, char *);
  int size = (int)s21_strlen(arg);
  for (int i = 0; i < size; i++) {
    str[i] = arg[i];
  }
  str[size] = '\0';
}

void get_c(const char *str, int *str_ch_count, char *arg, int *arg_ch_count) {
  arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
}

int get_d_str(const char *str, int *str_ch_count, char *arg, const int *cap,
              int *arg_ch_count, st_w *width) {
  int flag = 1;
  int error = 0;
  if (str[*str_ch_count] == '-') {
    arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
  } else if (str[*str_ch_count] == '+' && width->digit != 1) {
    *str_ch_count += 1;
  }
  while (flag && !error) {
    if (is_digit(str[*str_ch_count])) {
      if (width->flag && *arg_ch_count >= width->digit) {
        break;
      }
      arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
    } else {
      flag = 0;
    }
    if (*arg_ch_count == *cap) {
      error = 1;
    }
  }
  if (arg[0] == '\0') {
    error = 1;
  }
  return error;
}

void set_d(const char *arg, st_s *specifiers, st_l *length, va_list *input) {
  if (length->h == 1) {
    short *d = va_arg(*input, short *);
    *d = (short)get_int_from_str(arg);
  } else if (length->h == 1 && specifiers->u) {
    unsigned short *d = va_arg(*input, unsigned short *);
    *d = (unsigned short)get_int_from_str(arg);
  } else if (length->h == 2 && specifiers->u) {
    unsigned char *d = va_arg(*input, unsigned char *);
    *d = (unsigned char)get_int_from_str(arg);
  } else if (!length->h && !length->l && !length->L && specifiers->u) {
    unsigned int *d = va_arg(*input, unsigned int *);
    *d = (unsigned int)get_int_from_str(arg);
  } else if (length->l == 2 && (specifiers->d || specifiers->i)) {
    long long *d = va_arg(*input, long long *);
    *d = get_int_from_str(arg);
  } else if (length->l == 2 && specifiers->u) {
    unsigned long long *d = va_arg(*input, unsigned long long *);
    *d = (unsigned long long)get_int_from_str(arg);
  } else {
    int *d = va_arg(*input, int *);
    *d = (int)get_int_from_str(arg);
  }
}

int get_f_str(const char *str, int *str_ch_count, char *arg, const int *cap,
              int *arg_ch_count, st_w *width) {
  int while_flag = 1;
  int point_flag = 1;
  int e_flag = 1;
  int error = 0;

  if (check_inf(str, *str_ch_count)) {
    arg[(*arg_ch_count)++] = 'I';
    arg[(*arg_ch_count)++] = 'N';
    arg[(*arg_ch_count)++] = 'F';
    *str_ch_count += 3;
  } else if (check_NaN(str, *str_ch_count)) {
    arg[(*arg_ch_count)++] = 'N';
    arg[(*arg_ch_count)++] = 'A';
    arg[(*arg_ch_count)++] = 'N';
    *str_ch_count += 3;
  } else {
    if (str[*str_ch_count] == '-') {
      arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
    } else if (str[*str_ch_count] == '+' && width->digit != 1) {
      *str_ch_count += 1;
    }
    while (while_flag && !error) {
      if (width->flag && *arg_ch_count == width->digit) {
        break;
      }
      if (is_digit(str[*str_ch_count])) {
        arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
      } else if (str[*str_ch_count] == '.' && point_flag &&
                 *arg_ch_count != 0) {
        point_flag = 0;
        arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
      } else if ((str[*str_ch_count] == 'e' || str[*str_ch_count] == 'E') &&
                 (e_flag && (str[(*str_ch_count) + 1] == '+' ||
                             str[(*str_ch_count) + 1] == '-')) &&
                 is_digit(str[(*str_ch_count) + 2])) {
        e_flag = 0;
        for (int i = 0; i < 3; i++) {
          arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
        }
      } else {
        while_flag = 0;
      }
      if (*arg_ch_count == *cap) {
        error = 1;
      }
    }
    if (arg[0] == '\0') {
      error = 1;
    }
  }
  return error;
}

int check_inf(const char *str, int str_ch_count) {
  int result = 0;
  if (str[str_ch_count] == 'i' || str[str_ch_count] == 'I')
    if (str[str_ch_count + 1] == 'n' || str[str_ch_count + 1] == 'N')
      if (str[str_ch_count + 2] == 'f' || str[str_ch_count + 2] == 'F')
        result = 1;
  return result;
}

int check_NaN(const char *str, int str_ch_count) {
  int result = 0;
  if (str[str_ch_count] == 'N' || str[str_ch_count] == 'n')
    if (str[str_ch_count + 1] == 'A' || str[str_ch_count + 1] == 'a')
      if (str[str_ch_count + 2] == 'N' || str[str_ch_count + 2] == 'n')
        result = 1;
  return result;
}

void set_f(const char *arg, st_l *length, va_list *input) {
  long double temp = get_ld_from_arg(arg);
  if (length->L) {
    long double *ld = va_arg(*input, long double *);
    *ld = temp;
  } else if (length->l) {
    double *d = va_arg(*input, double *);
    *d = (double)temp;
  } else {
    float *f = va_arg(*input, float *);
    *f = (float)temp;
  }
}

int get_o_str(const char *str, int *str_ch_count, char *arg, const int *cap,
              int *arg_ch_count, st_w *width) {
  int while_flag = 1;
  int error = 0;
  if (str[*str_ch_count] == '-') {
    arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
  }
  if (str[*str_ch_count] == '0') {
    *str_ch_count += 1;
    error -= 1;
  }
  int i = *str_ch_count;
  while (while_flag && error <= 0) {
    if (is_digit(str[*str_ch_count])) {
      if (width->flag && i >= width->digit) {
        break;
      }
      int temp = digit_to_int(str[*str_ch_count]);
      if (temp == 8 || temp == 9) {
        break;
      }
      arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
    } else {
      while_flag = 0;
    }
    if (*arg_ch_count == *cap) {
      error += 1;
    }
    i += 1;
  }
  if (arg[0] == '\0') {
    error += 1;
  }
  error = error == -1 ? 0 : error;
  return error;
}

int get_x_str(const char *str, int *str_ch_count, char *arg, const int *cap,
              int *arg_ch_count, st_w *width) {
  int while_flag = 1;
  int error = 0;
  if (str[*str_ch_count] == '-') {
    arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
  }
  if (str[*str_ch_count] == '0') {
    arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
    if (str[*str_ch_count] == 'x' || str[*str_ch_count] == 'X') {
      *str_ch_count += 1;
    }
  }
  int i = *str_ch_count;
  while (while_flag && !error) {
    if (is_digit(str[*str_ch_count]) ||
        s21_strchr("AaBbCcDdEeFf", str[*str_ch_count])) {
      if (width->flag && i >= width->digit) {
        break;
      }
      arg[(*arg_ch_count)++] = str[(*str_ch_count)++];
    } else {
      while_flag = 0;
    }
    if (*arg_ch_count == *cap) {
      error = 1;
    }
    i++;
  }
  if (arg[0] == '\0') {
    error = 1;
  }
  return error;
}

void set_oxXp(char *arg, st_l *length, va_list *input, st_s *specifiers) {
  unsigned long long ll;
  if (specifiers->o) {
    ll = get_ll_from_o(arg);
  } else if (specifiers->x || specifiers->X || specifiers->p) {
    ll = get_ll_from_x(arg);
  }
  if (specifiers->p) {
    ll = get_neg_ll(arg, 3, ll);
    void **d = va_arg(*input, void **);
    *d = (void *)ll;
  } else {
    if (length->h == 1) {
      unsigned short *d = va_arg(*input, unsigned short *);
      ll = get_neg_ll(arg, 1, ll);
      *d = (unsigned short)ll;
    } else if (length->h == 2) {
      unsigned char *d = va_arg(*input, unsigned char *);
      ll = get_neg_ll(arg, 2, ll);
      *d = (unsigned char)ll;
    } else if (length->l == 1) {
      unsigned long *d = va_arg(*input, unsigned long *);
      ll = get_neg_ll(arg, 3, ll);
      *d = (unsigned long)ll;
    } else if (length->l == 2) {
      ll = get_neg_ll(arg, 3, ll);
      unsigned long long *d = va_arg(*input, unsigned long long *);
      *d = ll;
    } else {
      unsigned int *d = va_arg(*input, unsigned int *);
      ll = get_neg_ll(arg, 4, ll);
      *d = (unsigned int)ll;
    }
  }
}

unsigned long long get_ll_from_x(char *arg) {
  unsigned long long ll = 0;
  int size_arg = (int)s21_strlen(arg);
  for (int i = 0; arg[i]; i++) {
    int temp = size_arg - 1 - i;
    if (s21_strchr("AaBbCcDdEeFf", arg[temp])) {
      arg[temp] += arg[temp] < 96 ? 32 : 0;
      ll += (unsigned long long)((arg[temp] - 'a' + 10) * pow(16, i));
    } else {
      ll += (unsigned long long)((digit_to_int(arg[temp])) * pow(16, i));
    }
  }
  return ll;
}

unsigned long long get_ll_from_o(const char *arg) {
  unsigned long long ll = 0;
  int size_arg = (int)s21_strlen(arg);
  for (int i = 0; arg[i]; i++) {
    ll +=
        (unsigned long long)((digit_to_int(arg[size_arg - 1 - i])) * pow(8, i));
  }
  return ll;
}

unsigned long long get_neg_ll(const char *arg, int flag,
                              unsigned long long ll) {
  unsigned long long new_ll;
  long long temp[5] = {0, 65536, 256, 4611686018427387904, 4294967296};
  if (arg[0] == '-') {
    new_ll = temp[flag];
    if (flag == 3) {
      new_ll *= 4;
    }
    new_ll -= ll;
  } else {
    new_ll = ll;
  }
  return new_ll;
}

long double get_ld_from_arg(const char *arg) {
  long double ld = 0;
  if (s21_strcmp(arg, "INF") == 0) {
    ld = INFINITY;
  } else if (s21_strcmp(arg, "NAN") == 0) {
    ld = NAN;
  } else {
    int flag_e = 0;
    int flag_sign = 0;
    int flag_exp = 0;
    char buf[10] = "";
    int point = 0;
    int j = 0;
    int neg = 1;
    if (arg[0] == '-') {
      neg = -1;
    }
    for (int i = 0; arg[i] != '\0'; i++) {
      if (flag_exp) {
        buf[j++] = arg[i];
      } else {
        if (arg[i] == '.') {
          point = i;
        } else if (arg[i] == 'e' || arg[i] == 'E') {
          point = i - point - 1;
          i++;
          flag_e = 1;
          flag_sign = arg[i] == '-' ? 1 : 0;
          flag_exp = 1;
        } else {
          ld += digit_to_int(arg[i]);
          ld *= 10;
        }
        if (arg[i + 1] == '\0') {
          point = point == 0 ? point : i - point;
        }
      }
    }
    ld /= 10;
    ld /= (long double)pow(10, point);
    if (flag_e) {
      int exp = (int)get_int_from_str(buf);
      exp = flag_sign ? -exp : exp;
      ld *= pow(10, exp);
    }
    ld *= neg;
  }
  return ld;
}

long long get_int_from_str(const char *arg) {
  long long d = 0;
  int neg = 1;
  if (arg[0] == '-') {
    neg = -1;
  }
  for (int i = 0; arg[i]; i++) {
    d += digit_to_int(arg[i]);
    d *= 10;
  }
  d /= 10 * neg;
  return d;
}
