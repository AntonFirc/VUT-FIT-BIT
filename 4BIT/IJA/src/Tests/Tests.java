package Tests;
public class Tests {}
/*package Tests;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import BlockScheme.Block;
import BlockScheme.BlockPort;
import BlockScheme.Controller;
import org.junit.Before;
import org.junit.Test;
import org.junit.Assert;

public class Tests {

    @Before
    public void setUp() {

    }
    
    /**
     * Zakladni test implementace.
     *//*
    @Test
    public void test01() {

        Assert.assertEquals("Test stavu.", 2, 2);


        List<BlockPort> incomingPorts = new ArrayList<>();
        List<BlockPort> outcomingPorts = new ArrayList<>();

        HashMap<String, Double> typeIncoming1 = new HashMap<>();
        typeIncoming1.put("teplota", 2.0);
        HashMap<String, Double> typeOutcoming1 = new HashMap<>();
        typeOutcoming1.put("teplota", 4.0);

        BlockPort incoming1 = new BlockPort(typeIncoming1);
        BlockPort outcoming1 = new BlockPort(typeOutcoming1);

        incomingPorts.add(incoming1);
        outcomingPorts.add(outcoming1);

        Block block = new Block(incomingPorts, outcomingPorts);


        Assert.assertEquals("Test incoming portů", incomingPorts, block.incomingPorts);
        Assert.assertEquals("Test outcoming portů", outcomingPorts, block.outcomingPorts);
        Assert.assertEquals("Test typu incoming portu", typeIncoming1, block.incomingPorts.get(0).type);
        Assert.assertEquals("Test typu outcoming portu", typeOutcoming1, block.outcomingPorts.get(0).type);

    }

}
*/