import numpy as np
import pandas as pd


class Model:
	def __init__(self):
		self._params = dict()
		self._df_phi = None

	def from_file(self, filename):
		file = open(filename)
		lines = file.readlines()
		params_number = int(lines[3].split()[0])
		for line in lines[4:4 + params_number]:
			splitted = line.split()
			if 'skip' in splitted[0] or 'grid' in splitted[0]:
				splitted[-1] = int(splitted[-1])
			else:
				splitted[-1] = float(splitted[-1])
			self._params[splitted[0]] = splitted[-1]
		has_phi = lines[4 + params_number][0] == '+'
		file.close()
		self._params['dx'] = self._params['width'] / self._params['x_grid']
		self._params['dt'] = self._params['duration'] / self._params['t_grid']
		self._params['dx_data'] = self._params['dx'] * self._params['x_skip']
		self._params['dt_data'] = self._params['dt'] * self._params['t_skip']
		self._params['x_size'] = self._params['x_grid'] // self._params['x_skip'] + 1
		self._params['t_size'] = self._params['t_grid'] // self._params['t_skip'] + 1
		data = pd.read_csv(filename, sep=';', header=None,
			skiprows=6 + params_number)
		self._df_phi = None
		self._df_additional = data
		if has_phi:
			self._df_phi = data.iloc[:, :self._params['x_size']]
			self._df_additional = data.iloc[:, self._params['x_size']:]

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
	def df_additional(self):
		return self._df_additional

	@property
	def additional(self):
		return self._df_additional.iloc

	@property
	def xs(self):
		return np.arange(self._df_phi.shape[1]) * self._params['dx_data']

	@property
	def ts(self):
		return np.arange(self._df_phi.shape[0]) * self._params['dt_data']

	def t_index(self, t):
		return int(round(t / self._params['dt_data']))

	def phi_at_t(self, t):
		return self._df_phi.iloc[self.t_index(t)]

	def additional_at_t(self, t):
		return self._df_additional.iloc[self.t_index(t)]


def model_from_file(filename):
	model = Model();
	model.from_file(filename)
	return model
