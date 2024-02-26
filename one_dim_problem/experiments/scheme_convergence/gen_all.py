#!/usr/bin/env python3

from os import system
from sys import path
path.append("../scripts")
from model import Model

powers = [
	(1, 7), (1, 8), (1, 9), (1, 10), (1, 11), (1, 12), (1, 13),
	(2, 7), (2, 8), (2, 9), (2, 10), (2, 11), (2, 12), (2, 13),
	(3, 7), (3, 8), (3, 9), (3, 10), (3, 11), (3, 12), (3, 13),
	(4, 9), (4, 10), (4, 11), (4, 12), (4, 13),
	(5, 11), (5, 12), (5, 13),
	(6, 13),
	(7, 15)
]

for i, j in powers:
	x_grid = round(25 * 2 ** i)
	t_grid = round(25 * 2 ** j)
	system('./gen.py {} {} > params_gen.txt'.format(x_grid, t_grid))
	system('./one_dim params_gen.txt convergence/values_{}_{}.csv'.format(i, j))
	print('({}, {}) done'.format(i, j))
	model = Model()
	model.from_file('convergence/values_{}_{}.csv'.format(i, j))
	if model.df_phi.isna().sum().sum() > 0:
		print('Nans found!')
