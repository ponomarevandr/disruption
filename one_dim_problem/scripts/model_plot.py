import numpy as np
import matplotlib.pyplot as plt


def model_plot(model, times, **kwargs):
	if 'figsize' in kwargs:
		plt.figure(figsize=kwargs['figsize'])
	if 'title' in kwargs:
		plt.title(kwargs['title'])
	plt.xlabel('x')
	plt.ylabel('phi')
	t_indices = [int(t / model.params['data_dt']) for t in times]
	for i in t_indices:
	    plt.plot(np.arange(len(model.phi[i])) * model.params['data_dx'], model.phi[i])
	plt.legend(['t = {:.4f}'.format(i * model.params['data_dt']) for i in t_indices])
	plt.figtext(0, 0,
		'eps_0 = {}\n' +
		'delta = {}\n' +
		'l = {}\n' +
		'Gamma = {}\n' +
		'm = {}\n' +
		'Phi_coefficient = {}'.format(
			model.params['eps_0'],
			model.params['delta'],
			model.params['l'],
			model.params['Gamma'],
			model.params['m'],
			model.params['Phi_coefficient']
		),
		va='top'
	)
	plt.show()