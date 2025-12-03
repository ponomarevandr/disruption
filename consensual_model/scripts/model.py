import numpy as np
import pandas as pd


class Model:
	def __init__(self):
		self._params = dict()
		self._df_phi = None

	def from_file(self, filename):
		for name in ['eps_0', 'delta', 'l', 'Gamma', 'm', 'Phi_gradient']:
			self._params[name] = '[not a number]'
		file = open(filename)
		lines = file.readlines()
		header_lines = 2
		while True:
			splitted = lines[header_lines].split()
			if len(splitted) == 0:
				break
			header_lines += 1
			if splitted[-1] == '=' or splitted[0][0] == '[':
				continue
			for word in ['skip', 'grid', 'type']:
				if word in splitted[0]:
					splitted[-1] = int(splitted[-1])
			if (isinstance(splitted[-1], str)):
				splitted[-1] = float(splitted[-1])
			self._params[splitted[0]] = splitted[-1]
		file.close()
		if 't_0' not in self._params:
			self._params['t_0'] = 0.0
		self._params['dx'] = self._params['width'] / self._params['x_grid']
		self._params['dt'] = self._params['duration'] / self._params['t_grid']
		self._params['dx_data'] = self._params['dx'] * self._params['x_skip']
		self._params['x_size'] = self._params['x_grid'] // self._params['x_skip'] + 1
		data = pd.read_csv(filename, sep=';', header=None, skiprows=header_lines + 1)
		self._xs = np.arange(self._params['x_size']) * self._params['dx_data']
		self._dt_level = data.iloc[:, 0].to_numpy()
		self._dt_level[0] = 0
		self._ts = self._params['t_0'] + ((2**self._dt_level).cumsum() - 1) * self._params['dt']
		self._t_iterations = data.iloc[:, 1].to_numpy()
		self._df_phi = data.iloc[:, 2:self._params['x_size'] + 2].reset_index(drop=True)
		self._df_phi.columns = np.arange(self._df_phi.shape[1])
		self._energy_electrical = data.iloc[:, self._params['x_size'] + 2].to_numpy()
		self._energy_border = data.iloc[:, self._params['x_size'] + 3].to_numpy()
		self._energy_inner = data.iloc[:, self._params['x_size'] + 4].to_numpy()

	@property
	def params(self):
		return self._params
	
	@property
	def df_phi(self):
		return self._df_phi

	@property
	def phi(self):
		return self._df_phi.iloc

	@property
	def energy_electrical(self):
		return self._energy_electrical

	@property
	def energy_border(self):
		return self._energy_border

	@property
	def energy_inner(self):
		return self._energy_inner

	@property
	def xs(self):
		return self._xs

	@property
	def ts(self):
		return self._ts

	def t_index(self, t):
		index_first_greater = self._ts.searchsorted(t, 'right')
		if index_first_greater == len(self._ts):
			return index_first_greater - 1
		if self._ts[index_first_greater] - t < t - self._ts[index_first_greater - 1]:
			return index_first_greater
		else:
			return index_first_greater - 1

	def phi_at_t(self, t):
		return self._df_phi.iloc[self.t_index(t)]


def model_from_file(filename):
	model = Model();
	model.from_file(filename)
	return model
