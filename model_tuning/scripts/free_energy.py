import numpy as np


class FreeEnergy:
	def __init__(self, model):
		self._model = model

	def model_f(self, t):
		phi = self._model.phi_at_t(t).to_numpy()
		return 4 * phi**3 - 3 * phi**4

	def model_eps(self, t):
		return self._model.params['eps_0'] / (self.model_f(t) + self._model.params['delta'])

	def model_phi_x(self, t):
		phi = self._model.phi_at_t(t).to_numpy()
		phi_x = 0.5 * (phi[2:] - phi[:-2]) / self._model.params['dx_data']
		return np.concatenate(([phi_x[0]], phi_x, [phi_x[-1]]))

	def density_electrical(self, t):
		return -0.5 * self._model.params['Phi_gradient']**2 * self.model_eps(t)

	def density_depth(self, t):
		return self._model.params['Gamma'] * (1 - self.model_f(t)) / self._model.params['l']**2

	def density_surface(self, t):
		return 0.25 * self._model.params['Gamma'] * self.model_phi_x(t)**2

	def density_total(self, t):
		return self.density_electrical(t) + self.density_depth(t) + self.density_surface(t)

	def energy_total(self, t):
		density = self.density_total(t)
		return self._model.params['dx_data'] * \
			(density[1:-1].sum() + 0.5 * (density[0] + density[-1]))
