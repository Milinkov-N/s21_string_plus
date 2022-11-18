//
// Created by Maslyn Emperor on 11/8/22.
//

#ifndef SPRINTF__S21_SPRINTF_H_
#define SPRINTF__S21_SPRINTF_H_

typedef struct {
  int flag;
  int minus;
  int plus;
  int space;
  int hash;
  int zero;
  int error;
} st_f;

typedef struct {
  int flag;
  int digit;
} st_w;

typedef struct {
  int flag;
  int digit;
} st_a;

typedef struct {
  int flag;
  int h;
  int l;
  int L;
} st_l;

typedef struct {
  int c;
  int d;
  int i;
  int e;
  int E;
  int f;
  int g;
  int G;
  int o;
  int s;
  int u;
  int x;
  int X;
  int p;
  int n;
  int percent;
} st_s;

char *parse_str(const char *format, int *i, va_list *input, int char_count,
                int *free_flag, char formated_str[1024]);

void print_arg(char *str, int *char_count, char *arg);
void print_null(char *str, int *char_count);

void check_elements(const char *format, int *i, st_f *flag, st_w *width,
                    st_a *accuracy, st_l *length, va_list *input);

int is_flag(char format);

int is_width(char format);

int is_length(char format);

int is_specifier(char ch);

void get_flag(char format, st_f *flag);

void get_width(const char *format, int *i, st_w *width, va_list *input);

void get_accuracy(const char *format, int *i, st_a *accuracy, va_list *input);

void get_length(char format, st_l *length);

void get_specifier(char format, st_s *specifiers);

int parse_digit(const char *format, int *i);

int is_digit(char format);

int digit_to_int(char d);

char *get_formated_str(st_f *flag, st_w *width, st_a *accuracy, st_l *length,
                       st_s *specifiers, va_list *input, int char_count,
                       char formated_str[1024], int *free_flag);

void c_specif(va_list *input, char *arg, int *arg_len);

char *s_specif(va_list *input, char *arg, int *arg_len, st_a *accuracy);

void diu_specif(st_l *length, va_list *input, char *arg, int *arg_len,
                st_s *specifiers, st_a *accuracy);

void oxX_specif(st_f *flag, st_l *length, va_list *input, char *arg,
                int *arg_len, st_s *specifiers, st_a *accuracy);

void eEgG_specif(st_l *length, va_list *input, st_s *specifiers, char *arg,
                 int *arg_len, st_a *accuracy);

void n_specifiers(va_list *input, int char_count);

void s21_lltoa(long long i, char buf[64]);

void s21_ulltoa(unsigned long long i, char buf[64]);

void ftoa(long double ld, char *res, int flag_e, int flag_E, int accuracy);

void gtoa(long double ld, char *res, st_a *accuracy, st_s *specifiers);

void gtoa_accuracy(int six, int accuracy, char *res, int *temp, int more_six,
                   double *fpart, int digits_ipart, int digits_fpart);

int remake_double(long double *f);

void add_eE(int flag_e, int flag_E, char *res, int *temp, int e);

void del_zero(char *res, int last);

long long get_d(st_l *length, st_s *specifiers, va_list *input);

void o_specif(unsigned long long int *d);

void x_specif(unsigned long long int dd, char *buf, int flag_x);

char *make_width(st_w *width, st_f *flag, char *arg, int arg_len, int *cap,
                 int free_calloc);

char *make_flag(st_f *flag, char *arg, int *arg_len, int *cap,
                st_s *specifiers);

#endif  // SPRINTF__S21_SPRINTF_H_
