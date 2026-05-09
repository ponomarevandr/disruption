import numpy as np


def common_t_indices(dt_level_first, dt_level_second):
	dt_levels = [dt_level_first, dt_level_second]
	t_indices = ([], [])
	index = [0, 0]
	while index[0] < len(dt_levels[0]) and index[1] < len(dt_levels[1]):
		i_rough = int(dt_levels[0][index[0]] < dt_levels[1][index[1]])
		i_precise = 1 - i_rough
		scale_rough = 1 << dt_levels[i_rough][index[i_rough]]
		index[i_rough] += 1
		scale_precise = 0
		while scale_precise < scale_rough:
			scale_precise += 1 << dt_levels[i_precise][index[i_precise]]
			index[i_precise] += 1
		t_indices[i_rough].append(index[i_rough] - 1)
		t_indices[i_precise].append(index[i_precise] - 1)
	return t_indices

def get_skips(grid_first, grid_second):
	grid = min(grid_first, grid_second)
	if grid_first % grid != 0 or grid_second % grid != 0:
		return -1, -1, -1
	return grid, grid_first // grid, grid_second // grid

def phi_difference(model_first, model_second):
	t_indices_first, t_indices_second = \
		common_t_indices(model_first._dt_level, model_second._dt_level)
	_, x_skip_first, x_skip_second = \
		get_skips(model_first.df_phi.shape[1] - 1, model_second.df_phi.shape[1] - 1)
	values_numpy_first = model_first.df_phi.iloc[t_indices_first, ::x_skip_first].to_numpy()
	values_numpy_second = model_second.df_phi.iloc[t_indices_second, ::x_skip_second].to_numpy()
	return values_numpy_first - values_numpy_second, model_first.ts[t_indices_first]

def norm_uniform(values):
	return np.abs(values).max()

def optimal_norm_between(value, first, second):
	index_max = np.abs(second - first).argmax()
	coef = (value[index_max] - first[index_max]) / (second[index_max] - first[index_max])
	return norm_uniform(first + coef * (second - first) - value), coef

def metric_shifted(values_first, ts_first, values_second, ts_second):
	metric = np.zeros_like(ts_first)
	t_shift = np.zeros_like(ts_first)
	index_second = 0
	for index_first in range(len(ts_first)):
		metric_current, coef_current = optimal_norm_between(
			values_first.iloc[index_first],
			values_second.iloc[index_second],
			values_second.iloc[index_second + 1]
		)
		while index_second + 2 < len(ts_second) and coef_current > -0.5:
			metric_next, coef_next = optimal_norm_between(
				values_first.iloc[index_first],
				values_second.iloc[index_second + 1],
				values_second.iloc[index_second + 2]
			)
			if metric_next >= metric_current and coef_current < 1.5:
				break
			metric_current = metric_next
			coef_current = coef_next
			index_second += 1
		metric[index_first] = metric_current
		t_shift[index_first] = \
			ts_second[index_second] - ts_first[index_first] + \
			coef_current * (ts_second[index_second + 1] - ts_second[index_second])
	return metric, t_shift
