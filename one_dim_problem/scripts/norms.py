import numpy as np


def get_skips(grid_first, grid_second):
	grid = min(grid_first, grid_second)
	if grid_first % grid != 0 or grid_second % grid != 0:
		return -1, -1, -1
	return grid, grid_first // grid, grid_second // grid

def norm_continuous(model_first, model_second):
	_, x_skip_first, x_skip_second = \
		get_skips(model_first.params['x_grid_data'], model_second.params['x_grid_data'])
	_, t_skip_first, t_skip_second = \
		get_skips(model_first.params['t_grid_data'], model_second.params['t_grid_data'])
	if x_skip_first == -1 or t_skip_first == -1:
		return -1
	phi_first = model_first.phi[::t_skip_first, ::x_skip_first].to_numpy()
	phi_second = model_second.phi[::t_skip_second, ::x_skip_second].to_numpy()
	return np.max(np.abs(phi_first - phi_second))

def norm_L2(model_first, model_second):
	x_grid, x_skip_first, x_skip_second = \
		get_skips(model_first.params['x_grid_data'], model_second.params['x_grid_data'])
	t_grid, t_skip_first, t_skip_second = \
		get_skips(model_first.params['t_grid_data'], model_second.params['t_grid_data'])
	if x_skip_first == -1 or t_skip_first == -1:
		return -1
	phi_first = model_first.phi[::t_skip_first, ::x_skip_first].to_numpy()
	phi_second = model_second.phi[::t_skip_second, ::x_skip_second].to_numpy()
	coefficient = model_first.params['width'] * model_first.params['duration'] / x_grid / t_grid
	return np.sqrt(np.sum((phi_first - phi_second) ** 2) * coefficient)
