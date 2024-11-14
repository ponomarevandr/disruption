import numpy as np


def get_skips(grid_first, grid_second):
	grid = min(grid_first, grid_second)
	if grid_first % grid != 0 or grid_second % grid != 0:
		return -1, -1, -1
	return grid, grid_first // grid, grid_second // grid

def norm_continuous_1d(values_first, values_second):
	_, skip_first, skip_second = \
		get_skips(len(values_first) - 1, len(values_second) - 1)
	if skip_first == -1:
		return -1
	values_numpy_first = np.array(values_first[::skip_first])
	values_numpy_second = np.array(values_second[::skip_second])
	return np.max(np.abs(values_numpy_first - values_numpy_second))

def norm_continuous_2d(values_first, values_second):
	_, x_skip_first, x_skip_second = \
		get_skips(values_first.shape[1] - 1, values_second.shape[1] - 1)
	_, t_skip_first, t_skip_second = \
		get_skips(values_first.shape[0] - 1, values_second.shape[0] - 1)
	if x_skip_first == -1 or t_skip_first == -1:
		return -1
	values_numpy_first = values_first.iloc[::t_skip_first, ::x_skip_first].to_numpy()
	values_numpy_second = values_second.iloc[::t_skip_second, ::x_skip_second].to_numpy()
	return np.max(np.abs(values_numpy_first - values_numpy_second))

def norm_continuous(model_first, model_second):
	return norm_continuous_2d(model_first.df_phi, model_second.df_phi)

def norm_continuous_final(model_first, model_second):
	return norm_continuous_1d(model_first.df_phi.iloc[-1], model_second.df_phi.iloc[-1])

def norm_L2_1d(values_first, values_second, width):
	grid, skip_first, skip_second = \
		get_skips(len(values_first) - 1, len(values_second) - 1)
	if skip_first == -1:
		return -1
	values_numpy_first = np.array(values_first[::skip_first])
	values_numpy_second = np.array(values_second[::skip_second])
	coefficient = width / (grid + 1)
	return np.sqrt(np.sum((values_numpy_first - values_numpy_second) ** 2) * coefficient)

def norm_L2_2d(values_first, values_second, width, duration):
	x_grid, x_skip_first, x_skip_second = \
		get_skips(values_first.shape[1] - 1, values_second.shape[1] - 1)
	t_grid, t_skip_first, t_skip_second = \
		get_skips(values_first.shape[0] - 1, values_second.shape[0] - 1)
	if x_skip_first == -1 or t_skip_first == -1:
		return -1
	values_numpy_first = values_first.iloc[::t_skip_first, ::x_skip_first].to_numpy()
	values_numpy_second = values_second.iloc[::t_skip_second, ::x_skip_second].to_numpy()
	coefficient = width * duration / (x_grid + 1) / (t_grid + 1)
	return np.sqrt(np.sum((values_numpy_first - values_numpy_second) ** 2) * coefficient)

def norm_L2(model_first, model_second):
	return norm_L2_2d(model_first.df_phi, model_second.df_phi,
		model_first.params['width'], model_first.params['duration'])

def norm_L2_final(model_first, model_second):
	return norm_L2_1d(model_first.df_phi.iloc[-1], model_second.df_phi.iloc[-1],
		model_first.params['width'])
