#!/usr/bin/env python3

from sys import argv

x_grid, t_grid = map(int, argv[1:3])
x_skip = max(1, x_grid // 100)
t_skip = max(1, t_grid // 50)
print(
	('eps_0 = 0.2\n' + \
	'delta = 0.04\n' + \
	'l = 1.0\n' + \
	'Gamma = 1.0\n' + \
	'm = 0.5\n' + \
	'Phi_gradient = 4.8\n' + \
	'x_grid = {}\n' + \
	'x_skip = {}\n' + \
	't_grid = {}\n' + \
	't_skip = {}\n' + \
	'duration = 1.0\n' + \
	'width = 5.0').format(x_grid, x_skip, t_grid, t_skip)
)