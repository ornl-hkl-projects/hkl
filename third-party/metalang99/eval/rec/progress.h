#ifndef METALANG99_EVAL_REC_PROGRESS_H
#define METALANG99_EVAL_REC_PROGRESS_H

#define METALANG99_PRIV_REC_NEXT(next_lvl, choice)   METALANG99_PRIV_REC_NEXT_##choice(next_lvl)
#define METALANG99_PRIV_REC_NEXT_0continue(next_lvl) METALANG99_PRIV_REC_##next_lvl
#define METALANG99_PRIV_REC_NEXT_0stop(_next_lvl)    METALANG99_PRIV_REC_HALT

#define METALANG99_PRIV_REC_HALT(...) __VA_ARGS__

#endif // METALANG99_EVAL_REC_PROGRESS_H
