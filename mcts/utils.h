#pragma once
#include <algorithm>
#include <cstring>
#include <time.h>
#include "game_base.h"

#define OPTION_ARG(T, name)                                \
public:                                                    \
	inline auto name(const T &new_##name)->decltype(*this) \
	{ /* NOLINT */                                         \
		this->name##_ = new_##name;                        \
		return *this;                                      \
	}                                                      \
	inline auto name(T &&new_##name)->decltype(*this)      \
	{ /* NOLINT */                                         \
		this->name##_ = std::move(new_##name);             \
		return *this;                                      \
	}                                                      \
	inline const T &name() const noexcept                  \
	{ /* NOLINT */                                         \
		return this->name##_;                              \
	}                                                      \
	inline T &name() noexcept                              \
	{ /* NOLINT */                                         \
		return this->name##_;                              \
	}                                                      \
                                                           \
private:                                                   \
	T name##_ /* NOLINT */

#define OPTION_POINTER_ARG(T, name)                 \
public:                                             \
	inline auto name(T new_##name)->decltype(*this) \
	{ /* NOLINT */                                  \
		this->name##_ = new_##name;                 \
		return *this;                               \
	}                                               \
	inline T name() noexcept                        \
	{ /* NOLINT */                                  \
		return this->name##_;                       \
	}                                               \
                                                    \
private:                                            \
	T name##_ /* NOLINT */

float EvaluateResult(Game *state, Player turn);
