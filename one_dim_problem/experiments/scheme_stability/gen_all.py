#!/usr/bin/env python3

from os import system
from sys import path
path.append("../scripts")
from model import Model

for i in range(13):
	j = 0
	while True:
		x_grid = round(25 * 2 ** (i / 2))
		t_grid = round(25 * 2 ** (j / 2))
		system('./gen.py {} {} > params_gen.txt'.format(x_grid, t_grid))
		system('./one_dim params_gen.txt data_bounds/values_{}_{}.csv'.format(i, j))
		print('({}) done!'.format(i))
		model = Model()
		model.from_file('data_bounds/values_{}_{}.csv'.format(i, j))
		if model.df_phi.isna().sum().sum() == 0:
			print('No nans found!')
			break
		else:
			if j > 0:
				system('rm data_bounds/values_{}_{}.csv'.format(i, j - 1))
		j += 1
