import pandas as pd


class Model:
	PARAMETERS_NUMBER = 12

	def __init__(self):
		self._params = dict()
		self._df_phi = None

	def from_file(self, filename):
		file = open(filename)
		lines = file.readlines()
		for line in lines[1:self.PARAMETERS_NUMBER + 1]:
			splitted = line.split()
			if 'skip' in splitted[0]:
				splitted[-1] = int(splitted[-1])
			else:
				splitted[-1] = float(splitted[-1])
			self._params[splitted[0]] = splitted[-1]
		file.close()
		self._params['data_dx'] = self._params['dx'] * self._params['x_skip']
		self._params['data_dt'] = self._params['dt'] * self._params['t_skip']
		self._df_phi = pd.read_csv(filename, sep=';', header=None,
			skiprows=self.PARAMETERS_NUMBER + 1)

	@property
	def params(self):
		return self._params
	
	@property
	def df_phi(self):
		return self._df_phi

	@property
	def phi(self):
		return self._df_phi.iloc

	def t_index(self, t):
		return int(round(t / self._params['data_dt']))

	def phi_t(self, t):
		return self._df_phi.iloc[self.t_index(t)]
