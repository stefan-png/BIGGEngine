#pragma once

#define BIGG_STR (x) BIGG_STR_(x);
#define BIGG_STR_(x) #x

#define BIGG_CAT_(x, y) x ## y
#define BIGG_CAT (x, y) BIGG_CAT_(x, y)
