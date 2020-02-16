package BlockScheme;

import BlockScheme.Blocks.*;
import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.Cursor;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.input.*;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.scene.text.Text;
import javafx.stage.Stage;

import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Hlavní třída frontendu aplikace.
 * @author xfolty15, xfirca00
 *
 */
public class Main extends Application {

    public List<Block> blocks = new ArrayList<>(); //list of all blocks
    private Group root = new Group();   //create group for UI elements
    //variables for moving blocks
    private double orgSceneX, orgSceneY;
    private double orgTranslateX, orgTranslateY;
    private int block_cnt = 0; //used as block id
    List<Block> loaded = new ArrayList<>(); //list for loaded blocks

    /**
     * Hlavní initializátor frontendu aplikace
     * @param primaryStage
     */
    @Override
    public void start(Stage primaryStage) {

        //create UI menu and submenus
        MenuBar menuBar = new MenuBar();
        Menu menuScene = new Menu("Scene");
        Menu menuBlocks = new Menu("Blocks");

        MenuItem add = new MenuItem("Block 'ADD'");
        MenuItem sub = new MenuItem("Block 'SUB'");
        MenuItem mul = new MenuItem("Block 'MUL'");
        MenuItem div = new MenuItem("Block 'DIV'");
        MenuItem sqrt = new MenuItem("Block 'SQRT'");

        MenuItem exec = new MenuItem("RUN");
        MenuItem newScene = new MenuItem("New");
        MenuItem save = new MenuItem("Save");
        MenuItem load = new MenuItem("Load");

        menuBlocks.getItems().addAll(add, sub, mul, div, sqrt);
        menuScene.getItems().addAll(newScene, load, save, exec);
        menuBar.getMenus().addAll(menuScene, menuBlocks);


        root.getChildren().addAll(menuBar);

        primaryStage.setResizable(false);
        primaryStage.setScene(new Scene(root, 1280,720));
        primaryStage.setTitle("Blokové schéma - Projekt IJA");
        primaryStage.show();

        // new menu-button action
        newScene.setOnAction(new EventHandler<ActionEvent>() {
             @Override
             public void handle(ActionEvent event) {
                 clearBlocks();
             }
         });

        //Save menu-button action
        save.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {

                TextField tmp = new TextField();    //textfield for user defined name of savefile
                tmp.setLayoutX(600);
                tmp.setLayoutY(350);
                root.getChildren().add(tmp);
                tmp.setOnKeyPressed(new EventHandler<KeyEvent>() {
                    @Override
                    public void handle(KeyEvent event) {
                        if(event.getCode().equals(KeyCode.ENTER)) { //save after enter key pressed
                            System.out.println(tmp.getText());
                            try {
                                FileOutputStream fileOut =
                                        new FileOutputStream(tmp.getText() + ".ser");
                                ObjectOutputStream out = new ObjectOutputStream(fileOut);
                                for (int i = 0; i < blocks.size(); i++) {
                                    Block s_block;
                                    switch (blocks.get(i).block_type) { //create copy of block
                                        case 1: s_block = new Block_Add(blocks.get(i).incomingPorts, blocks.get(i).outcomingPorts);
                                                break;

                                        case 2: s_block = new Block_Substract(blocks.get(i).incomingPorts, blocks.get(i).outcomingPorts);
                                                break;

                                        case 3: s_block = new Block_Multiply(blocks.get(i).incomingPorts, blocks.get(i).outcomingPorts);
                                                break;

                                        case 4: s_block = new Block_Divide(blocks.get(i).incomingPorts, blocks.get(i).outcomingPorts);
                                                break;

                                        case 5: s_block = new Block_SQRT(blocks.get(i).incomingPorts, blocks.get(i).outcomingPorts);
                                                break;

                                        default: s_block = new Block(blocks.get(i).incomingPorts, blocks.get(i).outcomingPorts);

                                        }
                                    s_block.block_type = (blocks.get(i).block_type);
                                    s_block.ID = (blocks.get(i).ID);
                                    s_block.Ypos = blocks.get(i).visual.grp.getBoundsInParent().getMinY() + 3.5;
                                    s_block.Xpos = blocks.get(i).visual.grp.getBoundsInParent().getMinX() + 3.5;
                                    s_block.visual = null;
                                    out.writeObject(s_block);   //save block
                                }
                                out.close();
                                fileOut.close();
                                System.out.printf("Serialized data is saved in " + tmp.getText() + ".ser");
                            } catch (IOException i) {
                                i.printStackTrace();
                            }
                            root.getChildren().remove(tmp); //remove textfield
                        }
                    }
                });
            }
        });

        //load menu-button pressed
        load.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                clearBlocks();

                    loaded.clear(); //clear list of loaded blocks
                    File directory = new File(System.getProperty("user.dir"));
                    String filename[] = directory.list();

                    List <Text> saves = new ArrayList<>();

                    int Ypos = 200;

                    for( int i = 0; i<filename.length ; i++){   //scan directory for .ser files and print names
                        Pattern p = Pattern.compile(".*\\.ser$");
                        Matcher m = p.matcher(filename[i]);
                        if (m.find()) {
                            saves.add(new Text(600, Ypos, filename[i]));
                            Ypos += 40;
                            saves.get(saves.size() - 1).setFill(Color.GREEN);
                            saves.get(saves.size() - 1).setFont(Font.font("Arial", 30));
                            root.getChildren().add(saves.get(saves.size() - 1));
                            System.out.print(filename[i] + "\n");
                        }
                    }
                    TextField tmp = new TextField();
                    tmp.setLayoutX(600);
                    tmp.setLayoutY(Ypos - 30);
                    root.getChildren().add(tmp);
                    tmp.setOnKeyPressed(new EventHandler<KeyEvent>() {  //wait until enter pressed, load user defined save file and load scene
                        @Override
                        public void handle(KeyEvent event) {
                            if(event.getCode().equals(KeyCode.ENTER)) {
                                System.out.println(tmp.getText());
                                for (Text t: saves
                                     ) {
                                    root.getChildren().remove(t);
                                }
                                    try{    //load all blocks from .ser file
                                        FileInputStream fileIn = new FileInputStream(tmp.getText() + ".ser");
                                        root.getChildren().remove(tmp);
                                        ObjectInputStream in = new ObjectInputStream(fileIn);
                                        loaded.add((Block) in.readObject());
                                        while (loaded.get(loaded.size() - 1) != null) {
                                            loaded.add((Block) in.readObject());
                                        }
                                        in.close();
                                        fileIn.close();
                                    } catch (EOFException eof) {

                                    }
                                    catch (IOException excep) {
                                        excep.printStackTrace();
                                        return;
                                    } catch (ClassNotFoundException c) {
                                        System.out.println("Employee class not found");
                                        c.printStackTrace();
                                        return;
                                    }

                                System.out.println(loaded.size());

                                for (int j = 0; j < loaded.size(); j++) {   //draw representation of all loaded blocks


                                    System.out.println(loaded.get(j).Xpos);
                                    System.out.println(loaded.get(j).Ypos);

                                    blocks.add(loaded.get(j));

                                    loaded.get(j).visual = new Block_Visual(loaded.get(j).ID);

                                    loaded.get(j).visual.grp = new Group();

                                    loaded.get(j).visual.grp.setOnMousePressed(rectangleOnMousePressedEventHandler);
                                    loaded.get(j).visual.grp.setOnMouseDragged(rectangleOnMouseDraggedEventHandler);

                                    Rectangle body = new Rectangle(loaded.get(j).Xpos, loaded.get(j).Ypos, 80, 80);
                                    switch (loaded.get(j).block_type) {
                                        case 1: body.setFill(Color.BLUEVIOLET);
                                                break;

                                        case 2: body.setFill(Color.INDIANRED);
                                                break;

                                        case 3: body.setFill(Color.CADETBLUE);
                                                break;

                                        case 4: body.setFill(Color.LIGHTGOLDENRODYELLOW);
                                                break;

                                        case 5: body.setFill(Color.DARKORANGE);
                                                break;
                                    }
                                    body.setCursor(Cursor.MOVE);
                                    loaded.get(j).visual.grp.getChildren().add(body);

                                    Text mark;

                                    switch (loaded.get(j).block_type) {
                                        case 1: mark = new Text(body.getX() + 25, body.getY() + 50, "+");
                                            break;

                                        case 2: mark = new Text(body.getX() + 25, body.getY() + 50, "-");
                                            break;

                                        case 3: mark = new Text(body.getX() + 25, body.getY() + 50, "*");
                                            break;

                                        case 4: mark = new Text(body.getX() + 25, body.getY() + 50, "/");
                                            break;

                                        case 5: mark = new Text(body.getX() + 25, body.getY() + 50, "\u221A");
                                            break;

                                        default: mark = new Text(body.getX() + 25, body.getY() + 50, "+");
                                    }
                                    mark.setFont(Font.font("Verdana", 40));
                                    mark.setCursor(Cursor.MOVE);
                                    loaded.get(j).visual.grp.getChildren().add(mark);

                                    List<Circle> ports_in = new ArrayList<>();
                                    List<Circle> ports_out = new ArrayList<>();

                                    //create circles representing ports
                                    if (loaded.get(j).block_type != 5) {
                                        ports_in.add(new Circle(body.getX() + 3.5, body.getY() + 3.5, 7, Color.BLACK));
                                        loaded.get(j).visual.grp.getChildren().add(ports_in.get(ports_in.size() - 1));

                                        ports_in.add(new Circle(body.getX() + 3.5, body.getY() + 77.5, 7, Color.BLACK));
                                        loaded.get(j).visual.grp.getChildren().add(ports_in.get(ports_in.size() - 1));

                                        ports_out.add(new Circle(body.getX() + 76, body.getY() + 40, 7, Color.BLACK));
                                        loaded.get(j).visual.grp.getChildren().add(ports_out.get(ports_out.size() - 1));

                                        loaded.get(j).visual.in1 = new Text(body.getX() + 7, body.getY() + 18, String.valueOf(loaded.get(j).incomingPorts.get(0).type.get("value")));
                                        loaded.get(j).visual.in2 = new Text(body.getX() + 7, body.getY() + 70, String.valueOf(loaded.get(j).incomingPorts.get(1).type.get("value")));
                                        loaded.get(j).visual.out1 = new Text(body.getX() + 60, body.getY() + 32, String.valueOf(loaded.get(j).outcomingPorts.get(0).type.get("value")));

                                        loaded.get(j).visual.grp.getChildren().addAll(loaded.get(j).visual.in1, loaded.get(j).visual.in2, loaded.get(j).visual.out1);

                                    }
                                    else { //only for SQRT block
                                        ports_in.add(new Circle(body.getX() + 3.5, body.getY() + 40, 7, Color.BLACK));
                                        loaded.get(j).visual.grp.getChildren().add(ports_in.get(ports_in.size() - 1));

                                        ports_out.add(new Circle(body.getX() + 76, body.getY() + 40, 7, Color.BLACK));
                                        loaded.get(j).visual.grp.getChildren().add(ports_out.get(ports_out.size() - 1));

                                        loaded.get(j).visual.in1 = new Text(body.getX() + 7, body.getY() + 18, String.valueOf(loaded.get(j).incomingPorts.get(0).type.get("value")));
                                        loaded.get(j).visual.in2 = new Text(body.getX() + 7, body.getY() + 70, "0.0");
                                        loaded.get(j).visual.out1 = new Text(body.getX() + 60, body.getY() + 32, String.valueOf(loaded.get(j).outcomingPorts.get(0).type.get("value")));
                                        loaded.get(j).visual.grp.getChildren().addAll(loaded.get(j).visual.in1, loaded.get(j).visual.out1);


                                    }

                                    int finalJ = j;
                                    //define behavior of ports (drag and drop)
                                    ports_out.forEach(c -> c.setOnDragDetected(new EventHandler<MouseEvent>() {
                                        public void handle(MouseEvent event) {
                                            Dragboard db = c.startDragAndDrop(TransferMode.ANY);

                                            ClipboardContent content = new ClipboardContent();
                                            content.putString(loaded.get(finalJ).visual.out1.getText() + "@" + loaded.get(finalJ).ID);
                                            db.setContent(content);

                                            event.consume();
                                        }
                                    }));

                                    int port_cnt = 2;
                                    if (loaded.get(j).block_type == 5) {    //if SQRT block
                                        port_cnt = 1;
                                    }

                                    for (int i = 0; i < port_cnt; i++) {
                                        int finalI = i;
                                        ports_in.get(i).setOnDragOver(new EventHandler<DragEvent>() {
                                            public void handle(DragEvent event) {
                                                if (event.getGestureSource() != ports_in.get(finalI) &&
                                                        event.getDragboard().hasString()) {
                                                    event.acceptTransferModes(TransferMode.COPY_OR_MOVE);
                                                }
                                                event.consume();
                                            }
                                        });

                                        int finalJ1 = j;
                                        ports_in.get(i).setOnDragDropped(new EventHandler<DragEvent>() {
                                            public void handle(DragEvent event) {
                                                Dragboard db = event.getDragboard();
                                                boolean success = false;
                                                if (db.hasString()) {
                                                    String[] split = event.getDragboard().getString().split("@");
                                                    if (finalI == 0) {
                                                        loaded.get(finalJ1).visual.in1.setText(split[0]);
                                                        loaded.get(finalJ1).incomingPorts.get(0).con_block_id = Integer.parseInt(split[1]);
                                                        System.out.println("Port value(1) " + split[0] + " from block ID " + loaded.get(finalJ1).incomingPorts.get(0).con_block_id);
                                                    } else if (finalI == 1) {
                                                        loaded.get(finalJ1).visual.in2.setText(split[0]);
                                                        loaded.get(finalJ1).incomingPorts.get(1).con_block_id = Integer.parseInt(split[1]);

                                                        Group grp = new Group();

                                                        Line line = new Line(100, 100, 500, 500);
                                                      //  line.startXProperty().bind(loaded.get(finalJ1).visual.in2.xProperty());
                                                        //line.startYProperty().bind(loaded.get(finalJ1).visual.in2.yProperty());
                                                        //line.endXProperty().bind(loaded.get(finalJ1).visual.in2.xProperty());
                                                        //line.endYProperty().bind(loaded.get(finalJ1).visual.in2.yProperty());


                                                        root.getChildren().add(line);

                                                        System.out.println("Port value(2) " + split[0] + " from block ID " + loaded.get(finalJ1).incomingPorts.get(1).con_block_id);
                                                    }
                                                    success = true;
                                                }
                                                event.setDropCompleted(success);

                                                event.consume();
                                            }
                                        });

                                        int finalJ2 = j;
                                        ports_in.get(i).setOnMouseClicked(new EventHandler<MouseEvent>() {
                                            @Override
                                            public void handle(MouseEvent event) {
                                                if (event.getButton().equals(MouseButton.PRIMARY)) {
                                                    if (event.getClickCount() == 2) {
                                                        TextField tmp = new TextField();
                                                        tmp.setLayoutX(600);
                                                        tmp.setLayoutY(350);
                                                        System.out.println("Loaded size:" + loaded.size());
                                                        loaded.get(finalJ2).visual.grp.getChildren().add(tmp);
                                                        tmp.setOnKeyPressed(new EventHandler<KeyEvent>() {
                                                            @Override
                                                            public void handle(KeyEvent event) {
                                                                if (event.getCode().equals(KeyCode.ENTER)) {
                                                                    if (finalI == 0) {
                                                                        loaded.get(finalJ2).visual.in1.setText(tmp.getText());
                                                                        loaded.get(finalJ2).incomingPorts.get(0).type.put("value", Double.parseDouble(tmp.getText()));
                                                                    } else if (finalI == 1) {
                                                                        loaded.get(finalJ2).visual.in2.setText(tmp.getText());
                                                                        loaded.get(finalJ2).incomingPorts.get(1).type.put("value", Double.parseDouble(tmp.getText()));
                                                                    }
                                                                    System.out.println(tmp.getText());
                                                                    loaded.get(finalJ2).visual.grp.getChildren().remove(tmp);
                                                                }
                                                            }
                                                        });
                                                    }
                                                }
                                            }
                                        });

                                    }

                                    root.getChildren().addAll(loaded.get(j).visual.grp); //add to main group to be visible

                                }


                            }
                        }
                    });





        }});

        //add menu-button pressed
        add.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {

                List <BlockPort> bp_in = new ArrayList<>();
                List <BlockPort> bp_out = new ArrayList<>();

                HashMap<String, Double> typeIncoming1 = new HashMap<>();
                typeIncoming1.put("value", 0.0);
                HashMap<String, Double> typeIncoming2 = new HashMap<>();
                typeIncoming2.put("value", 0.0);
                bp_in.add(new BlockPort(typeIncoming1));
                bp_in.add(new BlockPort(typeIncoming2));

                HashMap<String, Double> typeOutcoming1 = new HashMap<>();
                typeOutcoming1.put("value", null);

                bp_out.add(new BlockPort(typeOutcoming1));

                Block block = new Block_Add(bp_in, bp_out);

                blocks.add(block); //add to list of all blocks
                block.ID = block_cnt++;
                block.block_type = 1;   //assign type (1 => block_add)

                block.visual = new Block_Visual(block.ID);  //create new object for visual representation

                block.visual.grp = new Group(); //create group for visual representation

                //event handlers for monig blocks
                block.visual.grp.setOnMousePressed(rectangleOnMousePressedEventHandler);
                block.visual.grp.setOnMouseDragged(rectangleOnMouseDraggedEventHandler);

                Rectangle body = new Rectangle(600, 320, 80, 80); //main body
                body.setFill(Color.BLUEVIOLET);
                body.setCursor(Cursor.MOVE);
                block.visual.grp.getChildren().add(body);

                Text plus = new Text(body.getX() + 25, body.getY() + 50, "+");  //graphical block identification
                plus.setFont(Font.font ("Verdana", 40));
                block.visual.grp.getChildren().add(plus);

                //lists for ports visual representation
                List<Circle> ports_in = new ArrayList<>();
                List<Circle> ports_out = new ArrayList<>();

                ports_in.add(new Circle(body.getX() + 3.5, body.getY() + 3.5, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_in.get(ports_in.size() - 1));

                ports_in.add( new Circle(body.getX() + 3.5, body.getY() + 77.5, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_in.get(ports_in.size() - 1));

                ports_out.add( new Circle(body.getX() + 76, body.getY() + 40, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_out.get(ports_out.size() - 1));

                //texts represnting actual value of port
                block.visual.in1 = new Text(body.getX() + 7,body.getY() + 18, String.valueOf(bp_in.get(0).type.get("value")));
                block.visual.in2 = new Text(body.getX() + 7,body.getY() + 70, String.valueOf(bp_in.get(1).type.get("value")));
                block.visual.out1 = new Text(body.getX() + 60,body.getY() + 32, String.valueOf(bp_out.get(0).type.get("value")));

                block.visual.grp.getChildren().addAll(block.visual.in1, block.visual.in2, block.visual.out1);

                //event handlers for ports representation
                ports_out.forEach(c -> c.setOnDragDetected(new EventHandler<MouseEvent>() {
                    public void handle(MouseEvent event) {
                        //detects drag and copies value of port and id of ports block
                        Dragboard db = c.startDragAndDrop(TransferMode.ANY);
                        ClipboardContent content = new ClipboardContent();
                        content.putString(block.visual.out1.getText() + "@" + block.ID);
                        db.setContent(content);

                        event.consume();
                    }
                }));

                portInOnDragDrop(block, ports_in);

                root.getChildren().addAll(block.visual.grp);
            }
        });
        //sub menu-button pressed
        sub.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                List <BlockPort> bp_in = new ArrayList<>();
                List <BlockPort> bp_out = new ArrayList<>();

                HashMap<String, Double> typeIncoming1 = new HashMap<>();
                typeIncoming1.put("value", 0.0);
                HashMap<String, Double> typeIncoming2 = new HashMap<>();
                typeIncoming2.put("value", 0.0);
                bp_in.add(new BlockPort(typeIncoming1));
                bp_in.add(new BlockPort(typeIncoming2));

                HashMap<String, Double> typeOutcoming1 = new HashMap<>();
                typeOutcoming1.put("value", 0.0);

                bp_out.add(new BlockPort(typeOutcoming1));

                Block block = new Block_Substract(bp_in, bp_out);

                blocks.add(block);
                block.ID = block_cnt++;
                block.block_type = 2;

                block.visual = new Block_Visual(block.ID);

                block.visual.grp = new Group();

                block.visual.grp.setOnMousePressed(rectangleOnMousePressedEventHandler);
                block.visual.grp.setOnMouseDragged(rectangleOnMouseDraggedEventHandler);

                Rectangle body = new Rectangle(600, 320, 80, 80);
                body.setFill(Color.INDIANRED);
                body.setCursor(Cursor.MOVE);
                block.visual.grp.getChildren().add(body);

                Text minus = new Text(body.getX() + 30, body.getY() + 50, "-");
                minus.setFont(Font.font ("Verdana", 40));
                block.visual.grp.getChildren().add(minus);

                List<Circle> ports_in = new ArrayList<>();
                List<Circle> ports_out = new ArrayList<>();

                ports_in.add(new Circle(body.getX() + 3.5, body.getY() + 3.5, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_in.get(ports_in.size() - 1));

                ports_in.add( new Circle(body.getX() + 3.5, body.getY() + 77.5, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_in.get(ports_in.size() - 1));

                ports_out.add( new Circle(body.getX() + 76, body.getY() + 40, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_out.get(ports_out.size() - 1));


                block.visual.in1 = new Text(body.getX() + 7,body.getY() + 18, String.valueOf(bp_in.get(0).type.get("value")));
                block.visual.in2 = new Text(body.getX() + 7,body.getY() + 70, String.valueOf(bp_in.get(1).type.get("value")));
                block.visual.out1 = new Text(body.getX() + 60,body.getY() + 32, String.valueOf(bp_out.get(0).type.get("value")));

                block.visual.grp.getChildren().addAll(block.visual.in1, block.visual.in2, block.visual.out1);

                ports_out.forEach(c -> c.setOnDragDetected(new EventHandler<MouseEvent>() {
                    public void handle(MouseEvent event) {
                        Dragboard db = c.startDragAndDrop(TransferMode.ANY);
                        ClipboardContent content = new ClipboardContent();
                        content.putString(block.visual.out1.getText() + "@" + block.ID);
                        db.setContent(content);

                        event.consume();
                    }
                }));

                portInOnDragDrop(block, ports_in);

                root.getChildren().addAll(block.visual.grp);

            }
        });

        mul.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                List <BlockPort> bp_in = new ArrayList<>();
                List <BlockPort> bp_out = new ArrayList<>();

                HashMap<String, Double> typeIncoming1 = new HashMap<>();
                typeIncoming1.put("value", 0.0);
                HashMap<String, Double> typeIncoming2 = new HashMap<>();
                typeIncoming2.put("value", 0.0);
                bp_in.add(new BlockPort(typeIncoming1));
                bp_in.add(new BlockPort(typeIncoming2));

                HashMap<String, Double> typeOutcoming1 = new HashMap<>();
                typeOutcoming1.put("value", 0.0);

                bp_out.add(new BlockPort(typeOutcoming1));

                Block block = new Block_Multiply(bp_in, bp_out);

                blocks.add(block);
                block.ID = block_cnt++;
                block.block_type = 3;

                block.visual = new Block_Visual(block.ID);

                block.visual.grp = new Group();

                block.visual.grp.setOnMousePressed(rectangleOnMousePressedEventHandler);
                block.visual.grp.setOnMouseDragged(rectangleOnMouseDraggedEventHandler);

                Rectangle body = new Rectangle(600, 320, 80, 80);
                body.setFill(Color.CADETBLUE);
                body.setCursor(Cursor.MOVE);
                block.visual.grp.getChildren().add(body);

                Text times = new Text(body.getX() + 30, body.getY() + 60, "*");
                times.setFont(Font.font ("Verdana", 40));
                block.visual.grp.getChildren().add(times);

                List<Circle> ports_in = new ArrayList<>();
                List<Circle> ports_out = new ArrayList<>();

                ports_in.add(new Circle(body.getX() + 3.5, body.getY() + 3.5, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_in.get(ports_in.size() - 1));

                ports_in.add( new Circle(body.getX() + 3.5, body.getY() + 77.5, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_in.get(ports_in.size() - 1));

                ports_out.add( new Circle(body.getX() + 76, body.getY() + 40, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_out.get(ports_out.size() - 1));


                block.visual.in1 = new Text(body.getX() + 7,body.getY() + 18, String.valueOf(bp_in.get(0).type.get("value")));
                block.visual.in2 = new Text(body.getX() + 7,body.getY() + 70, String.valueOf(bp_in.get(1).type.get("value")));
                block.visual.out1 = new Text(body.getX() + 60,body.getY() + 32, String.valueOf(bp_out.get(0).type.get("value")));

                block.visual.grp.getChildren().addAll(block.visual.in1, block.visual.in2, block.visual.out1);

                ports_out.forEach(c -> c.setOnDragDetected(new EventHandler<MouseEvent>() {
                    public void handle(MouseEvent event) {

                        Dragboard db = c.startDragAndDrop(TransferMode.ANY);


                        ClipboardContent content = new ClipboardContent();
                        content.putString(block.visual.out1.getText() + "@" + block.ID);
                        db.setContent(content);

                        event.consume();
                    }
                }));

                portInOnDragDrop(block, ports_in);

                root.getChildren().addAll(block.visual.grp);

            }
        });

        div.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                List <BlockPort> bp_in = new ArrayList<>();
                List <BlockPort> bp_out = new ArrayList<>();

                HashMap<String, Double> typeIncoming1 = new HashMap<>();
                typeIncoming1.put("value", 0.0);
                HashMap<String, Double> typeIncoming2 = new HashMap<>();
                typeIncoming2.put("value", 1.0);
                bp_in.add(new BlockPort(typeIncoming1));
                bp_in.add(new BlockPort(typeIncoming2));

                HashMap<String, Double> typeOutcoming1 = new HashMap<>();
                typeOutcoming1.put("value", 0.0);

                bp_out.add(new BlockPort(typeOutcoming1));

                Block block = new Block_Divide(bp_in, bp_out);

                blocks.add(block);
                block.ID = block_cnt++;
                block.block_type = 4;

                block.visual = new Block_Visual(block.ID);

                block.visual.grp = new Group();

                block.visual.grp.setOnMousePressed(rectangleOnMousePressedEventHandler);
                block.visual.grp.setOnMouseDragged(rectangleOnMouseDraggedEventHandler);

                Rectangle body = new Rectangle(600, 320, 80, 80);
                body.setFill(Color.LIGHTGOLDENRODYELLOW);
                body.setCursor(Cursor.MOVE);
                block.visual.grp.getChildren().add(body);

                Text divide = new Text(body.getX() + 30, body.getY() + 60, "/");
                divide.setFont(Font.font ("Verdana", 40));
                block.visual.grp.getChildren().add(divide);

                List<Circle> ports_in = new ArrayList<>();
                List<Circle> ports_out = new ArrayList<>();

                ports_in.add(new Circle(body.getX() + 3.5, body.getY() + 3.5, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_in.get(ports_in.size() - 1));

                ports_in.add( new Circle(body.getX() + 3.5, body.getY() + 77.5, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_in.get(ports_in.size() - 1));

                ports_out.add( new Circle(body.getX() + 76, body.getY() + 40, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_out.get(ports_out.size() - 1));


                block.visual.in1 = new Text(body.getX() + 7,body.getY() + 18, String.valueOf(bp_in.get(0).type.get("value")));
                block.visual.in2 = new Text(body.getX() + 7,body.getY() + 70, String.valueOf(bp_in.get(1).type.get("value")));
                block.visual.out1 = new Text(body.getX() + 60,body.getY() + 32, String.valueOf(bp_out.get(0).type.get("value")));

                block.visual.grp.getChildren().addAll(block.visual.in1, block.visual.in2, block.visual.out1);

                ports_out.forEach(c -> c.setOnDragDetected(new EventHandler<MouseEvent>() {
                    public void handle(MouseEvent event) {

                        Dragboard db = c.startDragAndDrop(TransferMode.ANY);


                        ClipboardContent content = new ClipboardContent();
                        content.putString(block.visual.out1.getText() + "@" + block.ID);
                        db.setContent(content);

                        event.consume();
                    }
                }));

                portInOnDragDrop(block, ports_in);

                root.getChildren().addAll(block.visual.grp);

            }
        });

        sqrt.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                List <BlockPort> bp_in = new ArrayList<>();
                List <BlockPort> bp_out = new ArrayList<>();

                HashMap<String, Double> typeIncoming1 = new HashMap<>();
                typeIncoming1.put("value", 0.0);
                HashMap<String, Double> typeIncoming2 = new HashMap<>();
                typeIncoming2.put("value", 0.0);
                bp_in.add(new BlockPort(typeIncoming1));
                bp_in.add(new BlockPort(typeIncoming2));

                HashMap<String, Double> typeOutcoming1 = new HashMap<>();
                typeOutcoming1.put("value", null);

                bp_out.add(new BlockPort(typeOutcoming1));

                Block block = new Block_SQRT(bp_in, bp_out);

                blocks.add(block);
                block.ID = block_cnt++;
                block.block_type = 5;

                block.visual = new Block_Visual(block.ID);

                block.visual.grp = new Group();

                block.visual.grp.setOnMousePressed(rectangleOnMousePressedEventHandler);
                block.visual.grp.setOnMouseDragged(rectangleOnMouseDraggedEventHandler);

                Rectangle body = new Rectangle(600, 320, 80, 80);
                body.setFill(Color.DARKORANGE);
                body.setCursor(Cursor.MOVE);
                block.visual.grp.getChildren().add(body);

                Text sqrt = new Text(body.getX() + 30, body.getY() + 50, "\u221A");
                sqrt.setFont(Font.font ("Verdana", 30));
                block.visual.grp.getChildren().add(sqrt);

                List<Circle> ports_in = new ArrayList<>();
                List<Circle> ports_out = new ArrayList<>();

                ports_in.add(new Circle(body.getX() + 3.5, body.getY() + 40, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_in.get(ports_in.size() - 1));

                ports_out.add( new Circle(body.getX() + 76, body.getY() + 40, 7, Color.BLACK));
                block.visual.grp.getChildren().add(ports_out.get(ports_out.size() - 1));


                block.visual.in1 = new Text(body.getX() + 7,body.getY() + 18, String.valueOf(bp_in.get(0).type.get("value")));
                block.visual.in2 = new Text(body.getX() + 7,body.getY() + 70, "0");
                block.visual.out1 = new Text(body.getX() + 60,body.getY() + 32, String.valueOf(bp_out.get(0).type.get("value")));

                block.visual.grp.getChildren().addAll(block.visual.in1, block.visual.out1);

                ports_out.forEach(c -> c.setOnDragDetected(new EventHandler<MouseEvent>() {
                    public void handle(MouseEvent event) {

                        Dragboard db = c.startDragAndDrop(TransferMode.ANY);


                        ClipboardContent content = new ClipboardContent();
                        content.putString(block.visual.out1.getText() + "@" + block.ID);
                        db.setContent(content);

                        event.consume();
                    }
                }));

                ports_in.get(0).setOnDragOver(new EventHandler<DragEvent>() {
                    public void handle(DragEvent event) {
                        if (event.getGestureSource() != ports_in.get(0) &&
                                event.getDragboard().hasString()) {
                            event.acceptTransferModes(TransferMode.COPY_OR_MOVE);
                        }
                        event.consume();
                    }
                });

                ports_in.get(0).setOnDragDropped(new EventHandler<DragEvent>() {
                    public void handle(DragEvent event) {
                        Dragboard db = event.getDragboard();
                        boolean success = false;
                        if (db.hasString()) {
                            String[] split = event.getDragboard().getString().split("@");
                            block.visual.in1.setText(split[0]);
                            block.incomingPorts.get(0).con_block_id = Integer.parseInt(split[1]);
                            System.out.println("Port value " + split[0] + " from block ID " + block.incomingPorts.get(0).con_block_id);

                            success = true;

                            System.out.println("uff");


                            //root.getChildren().add(line);

                        }
                        event.setDropCompleted(success);

                        event.consume();
                    }
                });

                ports_in.get(0).setOnMouseClicked(new EventHandler<MouseEvent>() {
                    @Override
                    public void handle(MouseEvent event) {
                        if(event.getButton().equals(MouseButton.PRIMARY)){
                            if(event.getClickCount() == 2){
                                TextField tmp = new TextField();
                                tmp.setLayoutX(600);
                                tmp.setLayoutY(350);
                                block.visual.grp.getChildren().add(tmp);
                                tmp.setOnKeyPressed(new EventHandler<KeyEvent>() {
                                    @Override
                                    public void handle(KeyEvent event) {
                                        if(event.getCode().equals(KeyCode.ENTER)) {
                                            block.visual.in1.setText(tmp.getText());
                                            block.incomingPorts.get(0).type.put("value", Double.parseDouble(tmp.getText()));
                                            System.out.println(tmp.getText());
                                            block.visual.grp.getChildren().remove(tmp);
                                        }
                                    }
                                });
                            }
                        }
                    }
                });


                root.getChildren().addAll(block.visual.grp);

            }
        });

        exec.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                boolean everCounted = false;
                do {
                    System.out.println("Counting again...");

                    boolean counted = true;
                    for (Block block : blocks) { //for each block on scene
                        if (block.outcomingPorts.get(0).type.get("value") == null) {
                            counted = false;
                            break;
                        }
                    }
                    if (counted == true && everCounted) {
                        break;
                    }

                    everCounted = true;
                    System.out.println("Counting again...");

                    for (Block block : blocks) { //for each block on scene
                        //System.out.println("BLock ID " + block.ID + ": Port 1 IN: " + block.incomingPorts.get(0).type.get("value") + " from " + block.incomingPorts.get(0).con_block_id);
                        //System.out.println("Block ID " + block.ID + ": Port 2 IN: " + block.incomingPorts.get(1).type.get("value") + " from " + block.incomingPorts.get(1).con_block_id);
                        //System.out.println("Port 1 OUT: " + block.outcomingPorts.get(0).type.get("value"));

                        //if con_block_id == -1 port does not load its value from other block
                        if (block.incomingPorts.get(0).con_block_id == -1) {
                            block.incomingPorts.get(0).type.put("value", Double.parseDouble(block.visual.in1.getText()));
                        } else {
                            block.incomingPorts.get(0).type.put("value", blocks.get(block.incomingPorts.get(0).con_block_id).outcomingPorts.get(0).type.get("value"));
                        }
                        //if con_block_id == -1 port does not load its value from other block
                        if (block.incomingPorts.get(1).con_block_id == -1) {
                            block.incomingPorts.get(1).type.put("value", Double.parseDouble(block.visual.in2.getText()));
                        } else {
                            block.incomingPorts.get(1).type.put("value", blocks.get(block.incomingPorts.get(1).con_block_id).outcomingPorts.get(0).type.get("value"));
                        }

                        //System.out.println("After : Port 1 IN: " + block.incomingPorts.get(0).type.get("value"));
                        //System.out.println("After : Port 2 IN: " + block.incomingPorts.get(1).type.get("value"));
                        if (block.incomingPorts.get(0).type.get("value") != null) {
                            block.visual.in1.setText(block.incomingPorts.get(0).type.get("value").toString());
                        }

                        if (block.incomingPorts.get(1).type.get("value") != null) {
                            block.visual.in2.setText(block.incomingPorts.get(1).type.get("value").toString());
                        }

                        try {
                            block.execute(); //execute calculation
                            block.visual.out1.setText(block.outcomingPorts.get(0).type.get("value").toString());

                        } catch(NullPointerException e) {

                        }

                        //System.out.println("After : Port 1 OUT: " + block.outcomingPorts.get(0).type.get("value"));


                    }
                }while(true);
            }
        });

    }
    public void clearBlocks() {
        for (int j = 0; j < blocks.size(); j++) {   //draw representation of all loaded blocks

            blocks.get(j).visual.grp.getChildren().clear();
        }

        blocks.clear();
        for (int j = 0; j < loaded.size(); j++) {   //draw representation of all loaded blocks
            //System.out.write(loaded.get(j).visual.grp.getChildren().get(0).hashCode());
            loaded.get(j).visual.grp.getChildren().clear();
        }
        loaded.clear();
    }

    /**
     * Funkce volaná pro zadefinování eventů přidaného bloku
     * @param block
     * @param ports_in
     */
    public void portInOnDragDrop(Block block, List<Circle> ports_in) {
        for (int i = 0; i < 2; i++) { // for each incomingport 1 and incomingport 2
            int finalI = i; //auxiliary variable, identifies port 1
            ports_in.get(i).setOnDragOver(new EventHandler<DragEvent>() {
                public void handle(DragEvent event) {
                    //make incoming port accept dragged data
                    if (event.getGestureSource() != ports_in.get(finalI) &&
                            event.getDragboard().hasString()) {
                        event.acceptTransferModes(TransferMode.COPY_OR_MOVE);
                    }
                    event.consume();
                }
            });

            ports_in.get(i).setOnDragDropped(new EventHandler<DragEvent>() {
                public void handle(DragEvent event) {
                    //when dragged data are dropped on port, get value and ID of source block
                    Dragboard db = event.getDragboard();
                    boolean success = false;
                    if (db.hasString()) {
                        String[] split = event.getDragboard().getString().split("@");   //split incoming string for value and source block ID
                        if (finalI == 0) {
                            block.visual.in1.setText(split[0]);
                            block.incomingPorts.get(0).con_block_id = Integer.parseInt(split[1]);
                            System.out.println("Port values " + split[0] + " from block ID " + block.incomingPorts.get(0).con_block_id);
                        }
                        else if (finalI == 1){
                            block.visual.in2.setText(split[0]);
                            block.incomingPorts.get(1).con_block_id = Integer.parseInt(split[1]);
                            System.out.println("Port values " + split[0] + " from block ID " + block.incomingPorts.get(1).con_block_id);

                        }
                        int blockid = Integer.parseInt(split[1]);
//                        Block otherBlock = blocks.get(blockid);
  //                      Line line = new Line(block.visual.in2.getX(), block.visual.in2.getY(), otherBlock.visual.out1.getX(), otherBlock.visual.out1.getY());

                        //  line.startXProperty().bind( block.visual.in2.xProperty());
                        //line.startYProperty().bind( block.visual.in2.yProperty());
                        //line.endXProperty().bind( block.visual.in2.xProperty());
                        //line.endYProperty().bind( block.visual.in2.yProperty());

                      //  block.visual.grp.getChildren().add(line);
                        //root.getChildren().add(line);


                        success = true;
                    }
                    event.setDropCompleted(success);

                    event.consume();
                }
            });

         ports_in.get(i).setOnMouseClicked(new EventHandler<MouseEvent>() {
             @Override
             public void handle(MouseEvent event) {
                 //on double-click open textfield and set new port value
                 if(event.getButton().equals(MouseButton.PRIMARY)){
                     if(event.getClickCount() == 2){
                         TextField tmp = new TextField();
                         tmp.setLayoutX(600);
                         tmp.setLayoutY(350);
                         block.visual.grp.getChildren().add(tmp);
                         tmp.setOnKeyPressed(new EventHandler<KeyEvent>() {
                             @Override
                             public void handle(KeyEvent event) {
                                 if(event.getCode().equals(KeyCode.ENTER)) { //set value after enter pressed
                                     if (finalI == 0) {
                                         block.visual.in1.setText(tmp.getText());
                                         block.incomingPorts.get(0).type.put("value", Double.parseDouble(tmp.getText()));
                                     }
                                     else if (finalI == 1) {
                                         block.visual.in2.setText(tmp.getText());
                                         block.incomingPorts.get(1).type.put("value", Double.parseDouble(tmp.getText()));
                                     }
                                     System.out.println(tmp.getText());
                                     block.visual.grp.getChildren().remove(tmp);
                                 }
                             }
                         });
                     }
                 }
             }
         });
        }
    }

    public static void main(String[] args) {
        launch(args);
    }

    //event handlers for block movement
    EventHandler<MouseEvent> rectangleOnMousePressedEventHandler =
            new EventHandler<MouseEvent>() {

                @Override
                public void handle(MouseEvent t) {
                    orgSceneX = t.getSceneX();
                    orgSceneY = t.getSceneY();
                    orgTranslateX = ((Group)(t.getSource())).getTranslateX();
                    orgTranslateY = ((Group)(t.getSource())).getTranslateY();
                }
            };

    EventHandler<MouseEvent> rectangleOnMouseDraggedEventHandler =
            new EventHandler<MouseEvent>() {

                @Override
                public void handle(MouseEvent t) {
                    double offsetX = t.getSceneX() - orgSceneX;
                    double offsetY = t.getSceneY() - orgSceneY;
                    double newTranslateX = orgTranslateX + offsetX;
                    double newTranslateY = orgTranslateY + offsetY;

                    ((Group)(t.getSource())).setTranslateX(newTranslateX);
                    ((Group)(t.getSource())).setTranslateY(newTranslateY);
                }
            };

}

