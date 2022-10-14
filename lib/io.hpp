#pragma once
#include <types.hpp>

static inline void outb(word port, byte val);

static inline byte inb(word port);

void MagicBreak();

void debugPrint(string str);