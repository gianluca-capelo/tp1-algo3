cin >> n >> R;
Pesos.assign(n, 0);
Resistencias.assign(n, 0);
for (int i = 0; i < n; ++i) cin >> Pesos[i] >> Resistencias[i];

def save_instanceCORRECT(dataset, instance_name, Pesos,Resistencias, R):
    with open(F"instancias/{dataset}/{instance_name}.txt", "w") as f:

        print(len(Pesos), R, file=f)

        for i in range(len(Pesos)-1):
            print(Pesos[i], file=f, end=" ")
            print(Resistencias[i], file=f)
