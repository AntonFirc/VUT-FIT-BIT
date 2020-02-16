package BlockScheme.Blocks;

import javafx.scene.Group;
import javafx.scene.text.Text;

//stores attributes needed to draw block
/**
 * Třída pro spojení frontendového blocku s backendovým
 * @author xfirca00
 */
public class Block_Visual{
    public int block_id;
    public Group grp;
    public Text in1;
    public Text in2;
    public Text out1;

    public Block_Visual(int id) {
        this.block_id = id;
    }
}
