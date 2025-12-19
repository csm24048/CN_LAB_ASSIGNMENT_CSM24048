from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import OVSController
from mininet.cli import CLI
from mininet.log import setLogLevel


class LeafSpineTopo(Topo):
    def __init__(self, k=4):
        """
        k = switch radix (must be even)
        """
        super().__init__()

        if k % 2 != 0:
            raise Exception("Radix k must be even")

        num_spines = k // 2
        num_leaves = k
        hosts_per_leaf = k // 2

        # Create spine switches
        spines = []
        for i in range(num_spines):
            spine = self.addSwitch(f's{i+1}')
            spines.append(spine)

        # Create leaf switches and hosts
        for i in range(num_leaves):
            leaf = self.addSwitch(f'l{i+1}')

            # Connect leaf to all spines
            for spine in spines:
                self.addLink(leaf, spine)

            # Add hosts to leaf
            for h in range(hosts_per_leaf):
                host = self.addHost(f'h{i+1}{h+1}')
                self.addLink(host, leaf)


def run():
    topo = LeafSpineTopo(k=4)   # change k to scale
    net = Mininet(topo=topo, controller=OVSController)
    net.start()
    print("Leaf-Spine topology running")
    CLI(net)
    net.stop()


if __name__ == '__main__':
    setLogLevel('info')
    run()

