#pragma once
#include <concepts>
#include <eigen3/Eigen/Dense>
#define __SCOPE_EXIT(func, LINE) const auto SCOPE_##LINE = ScopeGuard((func))
#define _SCOPE_EXIT(func, LINE) __SCOPE_EXIT((func), LINE)
#define SCOPE_EXIT(func) _SCOPE_EXIT((func), __LINE__)

template <std::invocable T>
class ScopeGuard
{
	const T& func;

public:
	explicit ScopeGuard(T func) : func(func) {}
	ScopeGuard(ScopeGuard const&) = delete;
	ScopeGuard(ScopeGuard&&) = delete;

	ScopeGuard& operator=(ScopeGuard const&) = delete;
	ScopeGuard& operator=(ScopeGuard&&) = delete;

	~ScopeGuard()
	{
		if constexpr (std::is_pointer_v<T>)
		{
			if (func)
				std::invoke(func);
		}
		else 
			std::invoke(func);
	}
};

template <size_t N, class T> requires (N%2 == 0)
inline Eigen::Matrix<T,N,3> RK4(Eigen::Matrix<T, N, 3> const& Y, T dt, Eigen::Vector<T, N/2> const &masses)
{
	const T G = 30.;
	
	auto F = [G, &masses](Eigen::Matrix<T, N, 3> Y) {
		Eigen::Vector<T, N / 2> A[N / 2][N / 2]{};
		for (int i = 0; i < N / 2; ++i)
			for (int j = i + 1; j < N / 2; ++j)
			{
				const auto r = Y.row(2 * j) - Y.row(2 * i);
				const auto t = (G / r.norm() / r.norm() / r.norm()) * r;
				A[i][j] = masses[j] * t;
				A[j][i] = -masses[i] * t;
			}
		Eigen::Matrix<T, N, 3> dY{};
		for (int i = 0; i < N / 2; ++i) {
			dY.row(2*i) = Y.row(2*i + 1);
			for (int j = 0; j < N / 2; ++j)
				dY.row(2 * i + 1) += A[i][j];
		}
		
		return dY;
	};
	
	auto k1 = F(Y);
	auto k2 = F(Y + dt/2 * k1);
	auto k3 = F(Y + dt/2 * k2);
	auto k4 = F(Y + dt * k3);

	return Y + dt / 6 * (k1 + 2. * k2 + 2. * k3 + k4);
}