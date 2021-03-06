
#pragma once

#include "properties/IndexInherit.hh"
#include "properties/NumericalFlat.hh"

/**
 \ingroup properties

 Turns a symbol into an accent. Accented objects inherit all properties
 and indices from the objects which they wrap. Here is an example with
 inherited coordinate dependence:
 \begin{screen}{1,2,3,4,5,6}
 \hat{#}::Accent.
 \partial{#}::PartialDerivative.
 A::Depends(\partial).
 \partial(A \hat{A} B):
 @prodrule!(%):
 @unwrap!(%);
 \partial(A) * A * B + A * \partial(\hat{A}) * B;
 \end{screen}
 Without the accent property, \verb|\hat{A}| object would be a
 completely independent object, unrelated to \verb|A|.
*/

class Accent : public PropertyInherit, public IndexInherit, public NumericalFlat, virtual public property {
	public:
		virtual std::string name() const;
};

