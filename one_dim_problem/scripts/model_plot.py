import matplotlib.pyplot as plt


def model_plot(model, times, print_params=True, **kwargs):
	if 'figsize' in kwargs:
		plt.figure(figsize=kwargs['figsize'])
	if 'title' in kwargs:
		plt.title(kwargs['title'])
	plt.xlabel('$x$')
	plt.ylabel('$\phi$')
	t_indices = [model.t_index(t) for t in times]
	for i in t_indices:
	    plt.plot(model.xs, model.phi[i])
	#plt.legend(['$t = {:.4f}$'.format(i * model.params['dt_data']) for i in t_indices])
	plt.legend(['$t = {:.2f}$'.format(i * model.params['dt_data']) for i in t_indices],
		bbox_to_anchor=(1.02, 1), loc="upper left")
	if print_params:
		plt.figtext(0, 0, (
				'eps_0 = {:.6e}\n' +
				'delta = {:.6e}\n' +
				'l = {:.6e}\n' +
				'Gamma = {:.6e}\n' +
				'm = {:.6e}\n' +
				'Phi_gradient = {:.6e}'
			).format(
				model.params['eps_0'],
				model.params['delta'],
				model.params['l'],
				model.params['Gamma'],
				model.params['m'],
				model.params['Phi_gradient']
			),
			va='top'
		)
		plt.figtext(0.5, 0, (
				'dt = {:.6e}\n' +
				'dx = {:.6e}\n' +
				'duration = {:.6e}\n' +
				'width = {:.6e}'
			).format(
				model.params['dt'],
				model.params['dx'],
				model.params['duration'],
				model.params['width']
			),
			va='top'
		)
	plt.show()
