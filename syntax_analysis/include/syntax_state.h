enum e_SynTaxState{
    SNTX_NUL,       ///空状态
    SNTX_SP,        ///空格
    SNTX_LF_HT,     ///换行并缩进，声明、函数定义、语句结束、都要置为此状态
    SNTX_DELAY      ///延迟到出去下一个但此后确定输出格式
};
