#ifndef TABLES_H_
#define TABLES_H_

// 初始置换
extern const char IP[];

// PC-1 置换
extern const char PC1[];

// PC-2 置换
extern const char PC2[];

// 移动位数
extern const char iteration_shift[];

// E-扩展置换
extern const char E[];

// S-盒替代
extern const char S[8][64];

// P-盒置换
extern const char P[];

// 逆置换
extern const char PI[];

#endif  // TABLES_H_