package BlockScheme.Blocks;

import BlockScheme.Block;
import BlockScheme.BlockPort;

import java.util.List;

import static java.lang.Math.sqrt;

/**
 * Třída pro sqrt
 * @author xfirca00
 */
public class Block_SQRT extends Block implements IBlock {
    public Block_SQRT(List<BlockPort> incomingPorts, List<BlockPort> outcomingPorts) {
        super(incomingPorts, outcomingPorts);
    }

    @Override
    public void execute() {
        double first = incomingPorts.get(0).type.get("value");
        System.out.println(first);
        double result = sqrt(first);
        System.out.println(result);
        outcomingPorts.get(0).type.put("value", result);
    }

}

